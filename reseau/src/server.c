/**
    @file server.c
    @author i-Charlys (CAILLON Charles)
    @author Fshimi-Hawlk
    @date 2026-03-31
    @brief Authoritative RUDP server with multi-module support.
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/select.h>
#include <stddef.h>

#include "APIs/generalAPI.h"
#include "APIs/gameRegistry.h"
#include "networkInterface.h"
#include "rudp_core.h"
#include "logger.h"

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
    MiniGame_Et                   gameId;     ///< Type of game (BINGO, CHESS, etc.)
    const GameServerInterface_St* module;     ///< Pointer to game logic interface
    void*                         state;      ///< Opaque pointer to game instance data
    char                          name[32];   ///< Display name (e.g. "Room #1")
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
static bool         g_broadcastHappened = false;    ///< Tracks if a response was sent during current packet processing

// 
// Helper functions
// 

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
    @param addr Client address
    @return Client index (0..MAX_CLIENTS-1) or -1 if server is full
*/
static int findOrCreateClient(struct sockaddr_in* addr) {
    // Look for existing client
    for (int i = 0; i < MAX_CLIENTS; ++i) {
        if (clients[i].active &&
            clients[i].address.sin_addr.s_addr == addr->sin_addr.s_addr &&
            clients[i].address.sin_port == addr->sin_port) {
            return i;
        }
    }

    // Create new client
    for (int i = 0; i < MAX_CLIENTS; ++i) {
        if (!clients[i].active) {
            memset(&clients[i], 0, sizeof(UDPClient_St));
            clients[i].active = true;
            clients[i].address = *addr;
            clients[i].roomId = 0; // Default to Lobby
            rudpInitConnection(&clients[i].rudpState);
            gettimeofday(&clients[i].lastSeen, NULL);

            log_info("Client slot %d allocated for %s:%d (Lobby)",
                     i, inet_ntoa(addr->sin_addr), ntohs(addr->sin_port));
            return i;
        }
    }

    log_warn("Server is full - rejected new client from %s:%d",
             inet_ntoa(addr->sin_addr), ntohs(addr->sin_port));
    return -1;
}

/**
    @brief Sends a minimal RUDP acknowledgment to a client.
    @param clientId Client index
*/
static void serverSendAck(int clientId) {
    if (clientId < 0 || clientId >= MAX_CLIENTS || !clients[clientId].active) return;

    RUDPHeader_St header;
    rudpGenerateHeader(&clients[clientId].rudpState, ACTION_CODE_ACK_ONLY, &header);
    header.sender_id = htons(999); // Server ID

    sendto(masterSocket, &header, sizeof(RUDPHeader_St), 0,
           (struct sockaddr*)&clients[clientId].address, sizeof(struct sockaddr_in));
}

/**
    @brief Broadcasts or unicasts a message to clients.
    @param roomId   >= 0 : Broadcast in the specific room (excludes excludeId).
                    UNICAST (-1) : Unicast to client excludeId.
    @param excludeId Client to exclude (broadcast) or target (unicast)
    @param action   Action code
    @param payload  Message payload
    @param len      Payload length
*/
static void serverBroadcast(int roomId, int excludeId, u8 action, const void* payload, u16 len) {
    g_broadcastHappened = true;
    u8 buffer[2048];
    if (len > MAX_PAYLOAD_SIZE) len = MAX_PAYLOAD_SIZE;

    for (int i = 0; i < MAX_CLIENTS; ++i) {
        if (!clients[i].active) continue;

        bool shouldSend = false;
        if (roomId == UNICAST) {
            if (i == excludeId) shouldSend = true;
        } else {
            // Broadcast to everyone in the room except excludeId
            if (clients[i].roomId == roomId && i != excludeId) shouldSend = true;
        }

        if (shouldSend) {
            RUDPHeader_St header;
            rudpGenerateHeader(&clients[i].rudpState, action, &header);
            
            // If unicast or global broadcast, sender is the server (999). 
            // If targeted broadcast, sender is excludeId.
            u16 finalSenderId = (roomId == UNICAST || excludeId == -1) ? 999 : (u16)excludeId;
            header.sender_id = htons(finalSenderId); 

            memcpy(buffer, &header, sizeof(RUDPHeader_St));
            if (len > 0 && payload != NULL) {
                memcpy(buffer + sizeof(RUDPHeader_St), payload, len);
            }

            sendto(masterSocket, buffer, sizeof(RUDPHeader_St) + len, 0,
                   (struct sockaddr*)&clients[i].address, sizeof(struct sockaddr_in));
        }
    }
}

/**
    @brief Handles server discovery queries from clients.
    @param clientAddr Address of querying client
*/
static void handleDiscoveryQuery(struct sockaddr_in* clientAddr) {
    log_debug("Handling discovery query from %s:%d", inet_ntoa(clientAddr->sin_addr), ntohs(clientAddr->sin_port));
    
    RUDPHeader_St response = {0};
    response.action = ACTION_CODE_DISCOVERY_INFO;
    response.sender_id = htons(999); // Special ID for server

    u8 buffer[512];
    memcpy(buffer, &response, sizeof(RUDPHeader_St));

    char info[256];
    int activeRoomCount = 0;
    for (int i = 0; i < MAX_ROOMS; i++) if (rooms[i].active) activeRoomCount++;

    snprintf(info, sizeof(info), "%s [%d Rooms Active]", SERVER_DISPLAY_NAME, activeRoomCount);
    memcpy(buffer + sizeof(RUDPHeader_St), info, strlen(info) + 1);

    sendto(masterSocket, buffer, sizeof(RUDPHeader_St) + strlen(info) + 1, 0,
           (struct sockaddr*)clientAddr, sizeof(struct sockaddr_in));
    fflush(stdout);
}

/**
    @brief Checks for empty rooms and destroys them.
*/
static void cleanupEmptyRooms(void) {
    for (int i = 1; i < MAX_ROOMS; i++) { // Skip Lobby (room 0)
        if (rooms[i].active) {
            bool hasPlayers = false;
            for (int j = 0; j < MAX_CLIENTS; j++) {
                if (clients[j].active && clients[j].roomId == i) {
                    hasPlayers = true;
                    break;
                }
            }

            if (!hasPlayers) {
                log_info("Room %d (%s) is empty, destroying instance...", i, rooms[i].name);
                if (rooms[i].module && rooms[i].module->destroy_instance) {
                    rooms[i].module->destroy_instance(rooms[i].state);
                }
                memset(&rooms[i], 0, sizeof(Room_St));
            }
        }
    }
}

/**
    @brief Checks for timed-out clients and disconnects them.
*/
static void checkTimeouts(void) {
    struct timeval now;
    gettimeofday(&now, NULL);

    for (int i = 0; i < MAX_CLIENTS; ++i) {
        if (clients[i].active) {
            long long elapsed = \
                (long long)(now.tv_sec - clients[i].lastSeen.tv_sec) * MACROSECONDS_IN_A_SECOND 
              + (now.tv_usec - clients[i].lastSeen.tv_usec);

            if (elapsed > CLIENT_TIMEOUT_US) {
                log_info("Client %d timed out", i);
                int rId = clients[i].roomId;

                // Notify others in the same room BEFORE marking as inactive
                serverBroadcast(rId, i, ACTION_CODE_QUIT_GAME, NULL, 0);
                
                clients[i].active = false;

                if (rooms[rId].active && rooms[rId].module && rooms[rId].module->on_player_leave) {
                    rooms[rId].module->on_player_leave(rooms[rId].state, i);
                }
            }
        }
    }
    cleanupEmptyRooms();
}

/**
    @brief Sends list of active rooms for a specific game to a client.
*/
static void sendRoomList(int clientId, MiniGame_Et gameId) {
    RoomInfo_St info[MAX_ROOMS];
    memset(info, 0, sizeof(info));
    int count = 0;

    for (int i = 0; i < MAX_ROOMS; i++) {
        if (rooms[i].active && (rooms[i].gameId == gameId || gameId == MINI_GAME_LOBBY)) {
            info[count].id = htons((u16)i);
            
            int pCount = 0;
            for (int j = 0; j < MAX_CLIENTS; j++) if (clients[j].active && clients[j].roomId == i) pCount++;
            
            info[count].playerCount = htons((u16)pCount);
            strncpy(info[count].name, rooms[i].name, 31);
            strncpy(info[count].creator, rooms[i].creatorName, 31);
            count++;
        }
    }

    serverBroadcast(UNICAST, clientId, ACTION_CODE_LOBBY_ROOM_INFO, info, (u16)(count * sizeof(RoomInfo_St)));
}

// 
// Main
// 

int main(void) {
    struct sockaddr_in serverAddr;
    memset(clients, 0, sizeof(clients));

    // Create socket
    if ((masterSocket = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Socket creation failed");
        exit(1);
    }

    int opt = 1;
    setsockopt(masterSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    int brd = 1;
    setsockopt(masterSocket, SOL_SOCKET, SO_BROADCAST, &brd, sizeof(brd));

    // Bind socket
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(SERVER_PORT);

    if (bind(masterSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        perror("Bind failed");
        exit(1);
    }

    setNonBlocking(masterSocket);

    // Initialize Lobby Room
    rooms[0].active = true;
    rooms[0].id = 0;
    rooms[0].gameId = MINI_GAME_LOBBY;
    rooms[0].module = getGameServerInterface(MINI_GAME_LOBBY);
    if (rooms[0].module && rooms[0].module->create_instance) {
        rooms[0].state = rooms[0].module->create_instance();
    } else {
        log_error("Lobby module does not implement create_instance or not found");
        exit(1);
    }
    strncpy(rooms[0].name, "Lobby Central", 31);

    log_info("SERVER STARTED ON PORT %d", SERVER_PORT);
    log_info("LOBBY ROOM INITIALIZED");
    log_debug("Internal sizes: RUDPHeader=%zu, RoomInfo=%zd", sizeof(RUDPHeader_St), sizeof(RoomInfo_St));

    long long next_tick = getTimeUs() + TICK_US;

    // Main loop
    while (1) {
        long long now = getTimeUs();

        if (now >= next_tick) {
            checkTimeouts();
            
            // Update all active rooms
            for (int i = 0; i < MAX_ROOMS; i++) {
                if (rooms[i].active && rooms[i].module && rooms[i].module->on_tick) {
                    rooms[i].module->on_tick(rooms[i].state);
                }
            }
            
            next_tick += TICK_US;
            if (next_tick < now) next_tick = now + TICK_US;
        }

        fd_set readfds;
        FD_ZERO(&readfds);
        FD_SET(masterSocket, &readfds);

        long long remaining = next_tick - now;
        if (remaining < 0) remaining = 0;

        struct timeval timeout;
        timeout.tv_sec = remaining / MACROSECONDS_IN_A_SECOND;
        timeout.tv_usec = remaining % MACROSECONDS_IN_A_SECOND;

        if (select(masterSocket + 1, &readfds, NULL, NULL, &timeout) > 0) {
            if (FD_ISSET(masterSocket, &readfds)) {
                struct sockaddr_in c_addr;
                socklen_t a_len = sizeof(c_addr);
                u8 buf[2048];
                ssize_t received;
                int packets_this_tick = 0;

                while (packets_this_tick < 256) {
                    received = recvfrom(masterSocket, buf, sizeof(buf), 0, (struct sockaddr*)&c_addr, &a_len);
                    if (received < 0) {
                        if (errno == EAGAIN || errno == EWOULDBLOCK) break;
                        break;
                    }

                    packets_this_tick++;

                    if (received < (ssize_t)sizeof(RUDPHeader_St)) continue;
                    RUDPHeader_St* h = (RUDPHeader_St*)buf;

                    // Log every incoming packet for visibility
                    log_debug("[NET] Received packet from %s:%d | Action: 0x%02X | Size: %zd", 
                              inet_ntoa(c_addr.sin_addr), ntohs(c_addr.sin_port), h->action, received);

                    // Server Discovery (Broadcast)
                    if (h->action == ACTION_CODE_DISCOVERY_QUERY) {
                        log_info("[DISCOVERY] Query from %s:%d. Sending Room Info...", inet_ntoa(c_addr.sin_addr), ntohs(c_addr.sin_port));
                        handleDiscoveryQuery(&c_addr);
                        continue;
                    }

                    // Strict Authentication Gate:
                    // Only allow findOrCreateClient for unknown IPs if the action is JOIN_GAME.
                    int clientId = -1;
                    for (int i = 0; i < MAX_CLIENTS; ++i) {
                        if (clients[i].active &&
                            clients[i].address.sin_addr.s_addr == c_addr.sin_addr.s_addr &&
                            clients[i].address.sin_port == c_addr.sin_port) {
                            clientId = i;
                            break;
                        }
                    }

                    if (clientId == -1) {
                        if (h->action != ACTION_CODE_JOIN_GAME) {
                            // Silently drop unauthenticated packets from unknown IPs
                            continue;
                        }
                        clientId = findOrCreateClient(&c_addr);
                    }

                    if (clientId == -1) {
                        log_warn("[NET] Rejected connection from %s:%d (Server Full)", inet_ntoa(c_addr.sin_addr), ntohs(c_addr.sin_port));
                        continue;
                    }

                    g_broadcastHappened = false;
                    bool isNew = rudpProcessIncoming(&clients[clientId].rudpState, h);
                    if (!isNew) {
                        serverSendAck(clientId);
                        continue;
                    }

                    gettimeofday(&clients[clientId].lastSeen, NULL);

                    if (h->action == ACTION_CODE_ACK_ONLY) {
                        continue;
                    }

                    int currentRoomId = clients[clientId].roomId;

                    if (currentRoomId < 0 || currentRoomId >= MAX_ROOMS) {
                        log_error("[NET] Client %d has invalid roomId %d", clientId, currentRoomId);
                        clients[clientId].roomId = 0;
                        currentRoomId = 0;
                    }

                    // Handle Join Game (Server handshake)
                    if (h->action == ACTION_CODE_JOIN_GAME) {
                        char proposedName[32];
                        if (received > (ssize_t)sizeof(RUDPHeader_St)) {
                            snprintf(proposedName, 31, "%s", (char*)buf + sizeof(RUDPHeader_St));
                        } else {
                            snprintf(proposedName, 31, "unnamed");
                        }

                        // Check for duplicate pseudo
                        bool duplicate = false;
                        for (int i = 0; i < MAX_CLIENTS; i++) {
                            if (clients[i].active && i != clientId && strcmp(clients[i].name, proposedName) == 0) {
                                duplicate = true;
                                break;
                            }
                        }

                        if (duplicate) {
                            log_warn("[JOIN] Rejected client %d: Duplicate pseudo '%s'", clientId, proposedName);
                            serverBroadcast(UNICAST, clientId, ACTION_CODE_JOIN_ERROR, "Pseudo already taken", 20);
                            clients[clientId].active = false; // Kick immediately
                            continue;
                        }

                        strncpy(clients[clientId].name, proposedName, 31);
                        log_info("[JOIN] Client %d ('%s') connected", clientId, clients[clientId].name);

                        u16 assigned_id = htons((u16)clientId);
                        serverBroadcast(UNICAST, clientId, ACTION_CODE_JOIN_ACK, &assigned_id, sizeof(u16));

                        if (rooms[currentRoomId].active && rooms[currentRoomId].module && rooms[currentRoomId].module->on_action) {
                            rooms[currentRoomId].module->on_action(rooms[currentRoomId].state, currentRoomId, clientId, h->action, buf + sizeof(RUDPHeader_St), (u16)(received - sizeof(RUDPHeader_St)), serverBroadcast);
                        }
                    }                    // Handle Room List Query
                    else if (h->action == ACTION_CODE_LOBBY_ROOM_QUERY) {
                        if (received >= (ssize_t)(sizeof(RUDPHeader_St) + 1)) {
                            MiniGame_Et gameId = buf[sizeof(RUDPHeader_St)];
                            log_info("[ROOMS] Client %d requested list for Game ID %d", clientId, gameId);
                            sendRoomList(clientId, gameId);
                        }
                    }
                    // Handle Room Switch / Create
                    else if (h->action == ACTION_CODE_LOBBY_SWITCH_GAME) {
                        if (received >= (ssize_t)(sizeof(RUDPHeader_St) + 2)) {
                            MiniGame_Et targetGameId = buf[sizeof(RUDPHeader_St)];
                            s8 targetRoomId = (s8)buf[sizeof(RUDPHeader_St) + 1];

                            log_info("[SWITCH] Client %d requests switch: Game %d, Room %d", clientId, targetGameId, targetRoomId);

                            int newRoomId = -1;

                            // Request to create new room
                            if (targetRoomId == -1) {
                                for (int i = 1; i < MAX_ROOMS; i++) {
                                    if (!rooms[i].active) {
                                        rooms[i].active = true;
                                        rooms[i].id = i;
                                        rooms[i].gameId = targetGameId;
                                        rooms[i].module = getGameServerInterface(targetGameId);
                                        if (rooms[i].module != NULL) {
                                            rooms[i].state = rooms[i].module->create_instance();
                                            rooms[i].hostId = clientId;
                                            strncpy(rooms[i].creatorName, clients[clientId].name, 31);
                                            snprintf(rooms[i].name, 31, "Room #%d (%s)", i, rooms[i].module->game_name);
                                            newRoomId = i;
                                        } else {
                                            rooms[i].active = false;
                                        }
                                        break;
                                    }
                                }
                            } else if (targetRoomId >= 0 && targetRoomId < MAX_ROOMS && rooms[targetRoomId].active) {
                                newRoomId = targetRoomId;
                            }

                            if (newRoomId != -1) {
                                // Notify old room peers
                                serverBroadcast(currentRoomId, clientId, ACTION_CODE_QUIT_GAME, NULL, 0);

                                // Notify old room module
                                if (rooms[currentRoomId].active && rooms[currentRoomId].module && rooms[currentRoomId].module->on_player_leave) {
                                    rooms[currentRoomId].module->on_player_leave(rooms[currentRoomId].state, clientId);
                                }

                                clients[clientId].roomId = newRoomId;
                                log_info("Client %d moved to room %d", clientId, newRoomId);

                                // Synthesize JOIN_GAME for the new room module
                                if (rooms[newRoomId].active && rooms[newRoomId].module && rooms[newRoomId].module->on_action) {
                                    rooms[newRoomId].module->on_action(
                                        rooms[newRoomId].state,
                                        newRoomId,
                                        clientId,
                                        ACTION_CODE_JOIN_GAME,
                                        clients[clientId].name,
                                        (u16)strlen(clients[clientId].name) + 1,
                                        serverBroadcast
                                    );
                                }

                                // Confirm switch to client
                                u8 switchPayload[2] = { (u8)rooms[newRoomId].gameId, (u8)newRoomId };
                                serverBroadcast(UNICAST, clientId, ACTION_CODE_LOBBY_SWITCH_GAME, switchPayload, 2);
                            }
                        }
                    }
                    // Handle Chat (Local to room)
                    else if (h->action == ACTION_CODE_LOBBY_CHAT) {
                        serverBroadcast(currentRoomId, clientId, ACTION_CODE_LOBBY_CHAT, buf + sizeof(RUDPHeader_St), (u16)(received - sizeof(RUDPHeader_St)));
                    }
                    // Handle explicit client quit
                    else if (h->action == ACTION_CODE_QUIT_GAME) {
                        log_info("Client %d explicitly quit", clientId);
                        clients[clientId].active = false;
                        serverBroadcast(currentRoomId, clientId, ACTION_CODE_QUIT_GAME, NULL, 0);
                        if (rooms[currentRoomId].active && rooms[currentRoomId].module && rooms[currentRoomId].module->on_player_leave) {
                            rooms[currentRoomId].module->on_player_leave(rooms[currentRoomId].state, clientId);
                        }
                        cleanupEmptyRooms();
                    }
                    // Route other actions to the client's current room module
                    else {
                        Room_St* r = &rooms[currentRoomId];
                        if (r->active && r->module && r->module->on_action) {
                            r->module->on_action(
                                r->state,
                                currentRoomId,
                                clientId,
                                h->action,
                                buf + sizeof(RUDPHeader_St),
                                (u16)(received - sizeof(RUDPHeader_St)),
                                serverBroadcast
                            );
                        }
                    }

                    if (!g_broadcastHappened) {
                        serverSendAck(clientId);
                    }
                }
            }
        }
    }

    return 0;
}

#define LOGGER_IMPLEMENTATION
#include "logger.h"
