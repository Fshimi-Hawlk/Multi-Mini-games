/**
    @file server.c
    @author Charles CAILLON
    @date 2026-03-31
    @date 2026-04-14
    @brief Authoritative RUDP server with multi-module support.
*/
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/select.h>
#include <stddef.h>
#include <signal.h>

#include "APIs/generalAPI.h"
#include "gameRegistry.h"
#include "networkInterface.h"
#include "logger.h"
#include "raylib.h"

// 
// Configuration constants
// 

#define MACROSECONDS_IN_A_SECOND 1000000LL
#define TICK_US                 16666                                   /**< Fréquence de mise à jour (60 Hz). */
#define CLIENT_TIMEOUT_US       (60 * MACROSECONDS_IN_A_SECOND)         /**< Délai de déconnexion automatique (60 secondes). */
#define SERVER_DISPLAY_NAME     "Multi-Mini-Games Server"
#define MAX_PAYLOAD_SIZE        (2048 - sizeof(RUDPHeader_St))          /**< Taille maximale de la charge utile. */
#define ACTION_CODE_ACK_ONLY    0x00                                    /**< RUDP-level acknowledgment only. */

// 
// Internal client representation
// 

/**
    @brief Server-side representation of a connected client.
*/
typedef struct {
    bool              active;           ///< True when slot is occupied
    struct sockaddr_in address;         ///< Client UDP endpoint
    RUDPConnection_St rudpState;        ///< Per-client RUDP state
    struct timeval    lastSeen;         ///< Last activity timestamp
    int               roomId;           ///< Current room (0 = Lobby)
    char              name[32];         ///< Player chosen name
} UDPClient_St;

/**
    @brief Server-side representation of a game room (instance).
*/
typedef struct {
    bool                          active;     ///< True when room is in use
    int                           id;         ///< Room index
    MiniGameId_Et                 gameId;     ///< Type of game (BINGO, CHESS, etc.)
    const GameServerInterface_St* module;     ///< Pointer to game logic interface
    void*                         state;      ///< Opaque pointer to game instance data
    char                          name[32];       ///< Display name (e.g. "Room #1")
    char                          creatorName[32]; ///< Name of the player who created it
    int                           hostId;     ///< Client ID of the host
} Room_St;

// 
// Globals
// 

#define MAX_ROOMS 16

static int          masterSocket = -1;              ///< Main server listening socket
static UDPClient_St clients[MAX_CLIENTS] = {0};     ///< Table of connected clients
static Room_St      rooms[MAX_ROOMS] = {0};         ///< Table of active game rooms
static volatile bool keepRunning = true;

// 
// Helper functions
// 

static void handle_sigint(int sig) {
    (void)sig;
    keepRunning = false;
}

/**
    @brief Returns current time in microseconds.
*/
static long long getTimeUs(void) {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (long long)tv.tv_sec * MACROSECONDS_IN_A_SECOND + tv.tv_usec;
}

/**
    @brief Sets a socket to non-blocking mode.
*/
static void setNonBlocking(int fd) {
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags != -1) fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

/**
    @brief Finds existing client by address or creates a new slot.
*/
static int findOrCreateClient(struct sockaddr_in* addr) {
    for (int i = 0; i < MAX_CLIENTS; ++i) {
        if (clients[i].active &&
            clients[i].address.sin_addr.s_addr == addr->sin_addr.s_addr &&
            clients[i].address.sin_port == addr->sin_port) {
            return i;
        }
    }

    for (int i = 0; i < MAX_CLIENTS; ++i) {
        if (!clients[i].active) {
            memset(&clients[i], 0, sizeof(UDPClient_St));
            clients[i].active = true;
            clients[i].address = *addr;
            clients[i].roomId = 0; 
            rudpInitConnection(&clients[i].rudpState);
            gettimeofday(&clients[i].lastSeen, NULL);
            return i;
        }
    }
    return -1;
}

static void serverSendAck(int clientId) {
    if (clientId < 0 || clientId >= MAX_CLIENTS || !clients[clientId].active) return;
    RUDPHeader_St header;
    rudpGenerateHeader(&clients[clientId].rudpState, ACTION_CODE_ACK_ONLY, &header);
    header.senderId = htons(999);
    sendto(masterSocket, &header, sizeof(RUDPHeader_St), 0,
           (struct sockaddr*)&clients[clientId].address, sizeof(struct sockaddr_in));
}

static void serverBroadcast(int roomId, int excludeId, u8 action, const void* payload, u16 len) {
    u8 buffer[2048];
    if (len > MAX_PAYLOAD_SIZE) len = MAX_PAYLOAD_SIZE;

    for (int i = 0; i < MAX_CLIENTS; ++i) {
        if (!clients[i].active) continue;
        bool shouldSend = (roomId == UNICAST) ? (i == excludeId) : (clients[i].roomId == roomId && i != excludeId);

        if (shouldSend) {
            RUDPHeader_St header;
            rudpGenerateHeader(&clients[i].rudpState, action, &header);
            u16 finalSenderId = (roomId == UNICAST || excludeId == -1) ? 999 : (u16)excludeId;
            header.senderId = htons(finalSenderId); 
            memcpy(buffer, &header, sizeof(RUDPHeader_St));
            if (len > 0 && payload != NULL) memcpy(buffer + sizeof(RUDPHeader_St), payload, len);
            sendto(masterSocket, buffer, sizeof(RUDPHeader_St) + len, 0,
                   (struct sockaddr*)&clients[i].address, sizeof(struct sockaddr_in));
        }
    }
}

static void handleDiscovery(struct sockaddr_in* clientAddr) {
    RUDPHeader_St response;
    memset(&response, 0, sizeof(response));
    response.action = ACTION_CODE_DISCOVERY_INFO;
    response.senderId = htons(999);
    u8 buffer[512];
    memcpy(buffer, &response, sizeof(RUDPHeader_St));
    const char* info = SERVER_DISPLAY_NAME;
    memcpy(buffer + sizeof(RUDPHeader_St), info, strlen(info) + 1);
    sendto(masterSocket, buffer, sizeof(RUDPHeader_St) + (u16)strlen(info) + 1, 0,
           (struct sockaddr*)clientAddr, sizeof(struct sockaddr_in));
}

static void checkTimeouts(void) {
    struct timeval now;
    gettimeofday(&now, NULL);
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i].active) {
            long long diff = (long long)(now.tv_sec - clients[i].lastSeen.tv_sec) * MACROSECONDS_IN_A_SECOND + (now.tv_usec - clients[i].lastSeen.tv_usec);
            if (diff > CLIENT_TIMEOUT_US) {
                log_info("Client %d timed out", i);
                int rId = clients[i].roomId;
                if (rId > 0 && rooms[rId].active && rooms[rId].module && rooms[rId].module->onPlayerLeave) {
                    rooms[rId].module->onPlayerLeave(rooms[rId].state, i);
                }
                clients[i].active = false;
                serverBroadcast(rId, i, ACTION_CODE_QUIT_GAME, NULL, 0);
            }
        }
    }
}

static void sendRoomList(int clientId) {
    RoomInfo_St info[MAX_ROOMS];
    int count = 0;
    for (int i = 0; i < MAX_ROOMS; i++) {
        if (rooms[i].active) {
            info[count].id = htons((u16)i);
            int pCount = 0;
            for (int j = 0; j < MAX_CLIENTS; j++) if (clients[j].active && clients[j].roomId == i) pCount++;
            info[count].playerCount = htons((u16)pCount);
            strncpy(info[count].name, rooms[i].name, 31);
            info[count].name[31] = '\0';
            strncpy(info[count].creator, rooms[i].creatorName, 31);
            info[count].creator[31] = '\0';
            count++;
        }
    }
    serverBroadcast(UNICAST, clientId, ACTION_CODE_LOBBY_ROOM_INFO, info, (u16)(count * sizeof(RoomInfo_St)));
}

int main(int argc, char* argv[]) {
    (void)argc; (void)argv;
    initLogger();
    log_info("SERVER STARTING...");

    masterSocket = socket(AF_INET, SOCK_DGRAM, 0);
    if (masterSocket < 0) { perror("socket"); return 1; }
    setNonBlocking(masterSocket);

    struct sockaddr_in serverAddr;
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(SERVER_PORT);

    if (bind(masterSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) { perror("bind"); return 1; }

    rooms[0].active = true;
    rooms[0].id = 0;
    rooms[0].gameId = MINI_GAME_ID_LOBBY;
    rooms[0].module = getGameServerInterface(MINI_GAME_ID_LOBBY);
    if (rooms[0].module) rooms[0].state = rooms[0].module->createInstance();
    strncpy(rooms[0].name, "Central Lobby", sizeof(rooms[0].name) - 1);

    signal(SIGINT, handle_sigint);
    long long next_tick = getTimeUs() + TICK_US;

    while (keepRunning) {
        long long now = getTimeUs();
        if (now >= next_tick) {
            checkTimeouts();
            for (int i = 0; i < MAX_ROOMS; i++) {
                if (rooms[i].active) {
                    // Check if room is empty (excluding lobby)
                    if (i > 0) {
                        bool empty = true;
                        for (int j = 0; j < MAX_CLIENTS; j++) {
                            if (clients[j].active && clients[j].roomId == i) {
                                empty = false;
                                break;
                            }
                        }
                        if (empty) {
                            log_info("Destroying empty room %d (%s)", i, rooms[i].name);
                            if (rooms[i].module && rooms[i].module->destroyInstance) {
                                rooms[i].module->destroyInstance(rooms[i].state);
                            }
                            memset(&rooms[i], 0, sizeof(Room_St));
                            continue;
                        }
                    }

                    if (rooms[i].module && rooms[i].module->onTick && rooms[i].state) {
                        rooms[i].module->onTick(rooms[i].state);
                    }
                }
            }
            next_tick += TICK_US;
        }

        fd_set readfds;
        FD_ZERO(&readfds);
        FD_SET(masterSocket, &readfds);
        struct timeval timeout = {0, 1000}; 

        if (select(masterSocket + 1, &readfds, NULL, NULL, &timeout) > 0) {
            u8 buf[2048];
            struct sockaddr_in clientAddr;
            socklen_t addrLen = sizeof(clientAddr);
            ssize_t received = recvfrom(masterSocket, buf, sizeof(buf), 0, (struct sockaddr*)&clientAddr, &addrLen);

            if (received >= (ssize_t)sizeof(RUDPHeader_St)) {
                RUDPHeader_St* h = (RUDPHeader_St*)buf;
                if (h->action == ACTION_CODE_DISCOVERY_QUERY) {
                    handleDiscovery(&clientAddr);
                    continue;
                }

                int clientId = findOrCreateClient(&clientAddr);
                if (clientId != -1 && rudpProcessIncoming(&clients[clientId].rudpState, h)) {
                    gettimeofday(&clients[clientId].lastSeen, NULL);
                    int currentRoomId = clients[clientId].roomId;

                    if (h->action == ACTION_CODE_LOBBY_ROOM_QUERY) sendRoomList(clientId);
                    else if (h->action == ACTION_CODE_QUIT_GAME) {
                        int rId = clients[clientId].roomId;
                        if (rId > 0 && rooms[rId].active && rooms[rId].module && rooms[rId].module->onPlayerLeave) {
                            rooms[rId].module->onPlayerLeave(rooms[rId].state, clientId);
                        }
                        clients[clientId].roomId = 0;
                        serverBroadcast(rId, clientId, ACTION_CODE_QUIT_GAME, NULL, 0);
                    }
                    else if (h->action == ACTION_CODE_JOIN_GAME) {
                        char proposedName[32] = {0};
                        if (received > (ssize_t)sizeof(RUDPHeader_St)) {
                            u16 nameLen = (u16)(received - sizeof(RUDPHeader_St));
                            if (nameLen > 31) nameLen = 31;
                            memcpy(proposedName, buf + sizeof(RUDPHeader_St), nameLen);
                        } else strncpy(proposedName, "unnamed", 31);

                        bool duplicate = false;
                        for (int i = 0; i < MAX_CLIENTS; i++) {
                            if (clients[i].active && i != clientId && strcmp(clients[i].name, proposedName) == 0) {
                                if (clients[i].address.sin_addr.s_addr == clients[clientId].address.sin_addr.s_addr) {
                                    clients[i].active = false;
                                } else { duplicate = true; break; }
                            }
                        }

                        if (duplicate) {
                            serverBroadcast(UNICAST, clientId, ACTION_CODE_JOIN_ERROR, "Pseudo taken", 13);
                            clients[clientId].active = false;
                            continue;
                        }
                        strncpy(clients[clientId].name, proposedName, 31);
                        clients[clientId].name[31] = '\0';
                        u16 assigned_id = htons((u16)clientId);
                        serverBroadcast(UNICAST, clientId, ACTION_CODE_JOIN_ACK, &assigned_id, sizeof(u16));
                    }
                    else if (h->action == ACTION_CODE_LOBBY_SWITCH_GAME) {
                        if (received >= (ssize_t)(sizeof(RUDPHeader_St) + 2)) {
                            MiniGameId_Et targetGameId = (MiniGameId_Et)buf[sizeof(RUDPHeader_St)];
                            s8 targetRoomId = (s8)buf[sizeof(RUDPHeader_St) + 1];

                            if (targetRoomId == -1) {
                                if (targetGameId != MINI_GAME_ID_LOBBY && targetGameId < __miniGameIdCount) {
                                    for (int i = 1; i < MAX_ROOMS; i++) {
                                        if (!rooms[i].active) {
                                            rooms[i].active = true;
                                            rooms[i].id = i;
                                            rooms[i].gameId = targetGameId;
                                            rooms[i].module = getGameServerInterface(targetGameId);
                                            if (rooms[i].module) {
                                                rooms[i].state = rooms[i].module->createInstance();
                                                rooms[i].hostId = clientId;
                                                strncpy(rooms[i].creatorName, clients[clientId].name, 31);
                                                snprintf(rooms[i].name, sizeof(rooms[i].name), "Room #%d", i);
                                                clients[clientId].roomId = i;
                                                u8 resp[2] = { (u8)targetGameId, (u8)i };
                                                serverBroadcast(UNICAST, clientId, ACTION_CODE_LOBBY_SWITCH_GAME, resp, 2);
                                            } else rooms[i].active = false;
                                            break;
                                        }
                                    }
                                }
                            } else if (targetRoomId >= 0 && targetRoomId < MAX_ROOMS && rooms[(int)targetRoomId].active) {
                                clients[clientId].roomId = (int)targetRoomId;
                                u8 resp[2] = { (u8)rooms[(int)targetRoomId].gameId, (u8)targetRoomId };
                                serverBroadcast(UNICAST, clientId, ACTION_CODE_LOBBY_SWITCH_GAME, resp, 2);
                            }
                        }
                    }
                    else {
                        Room_St* r = &rooms[currentRoomId];
                        if (r->active && r->module && r->module->onAction && r->state != NULL) {
                            r->module->onAction(r->state, currentRoomId, clientId, h->action, buf + sizeof(RUDPHeader_St), (u16)(received - sizeof(RUDPHeader_St)), serverBroadcast);
                        }
                    }
                    serverSendAck(clientId);
                }
            }
        }
    }

    if (masterSocket != -1) close(masterSocket);
    log_info("SERVER SHUTDOWN CLEANLY");
    return 0;
}

#define LOGGER_IMPLEMENTATION
#include "logger.h"
