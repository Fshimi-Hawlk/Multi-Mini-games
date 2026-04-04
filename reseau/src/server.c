/**
    @file server.c
    @author i-Charlys (CAILLON Charles)
    @date 2026-03-18
    @date 2026-03-30
    @brief Authoritative RUDP server with multi-module support.

    Manages multiple client connections, routes packets to the active game module
    (lobby or mini-games), handles discovery queries, timeouts and broadcasting.

    Communication diagram :
    [CLIENT 1] <----RUDP----> [ SERVEUR ] <----RUDP----> [CLIENT 2]
                                  |
                                  v
                       +---------------------+
                       |   ROUTING MODULES   |
                       +---------------------+
                       | action = SWITCH?    | ----> Change active_module
                       | action = GAME_DATA? | ----> on_action(active_game)
                       +---------------------+


    @note Currently supports lobby and King-for-Four. Easy to extend for more modules.
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/select.h>

#include "APIs/generalAPI.h"
#include "networkInterface.h"
#include "rudp_core.h"
#include "logger.h"

// ────────────────────────────────────────────────
// Configuration constants
// ────────────────────────────────────────────────

#define MACROSECONDS_IN_A_SECOND 1000000LL

#define SERVER_PORT             8080
#define CLIENT_TIMEOUT_US       (60 * MACROSECONDS_IN_A_SECOND)
#define SERVER_DISPLAY_NAME     "Multi-Mini-Games Server"
#define MAX_PAYLOAD_SIZE        (2048 - sizeof(RUDPHeader_St))

RUDPConnection_St serverConnection;

// ────────────────────────────────────────────────
// Action codes (lobby-specific)
// ────────────────────────────────────────────────

enum {
    ACTION_CODE_LOBBY_MOVE          = firstAvailableActionCode,
    ACTION_CODE_LOBBY_ROOM_QUERY,
    ACTION_CODE_LOBBY_ROOM_INFO,
    ACTION_CODE_LOBBY_CHAT,
    ACTION_CODE_LOBBY_SWITCH_GAME
};

// ────────────────────────────────────────────────
// Internal client representation
// ────────────────────────────────────────────────

/**
    @brief Server-side representation of a connected client.
*/
typedef struct {
    bool              active;           ///< True when slot is occupied
    struct sockaddr_in address;         ///< Client UDP endpoint
    RUDPConnection_St rudpState;        ///< Per-client RUDP state
    struct timeval    lastSeen;         ///< Last activity timestamp
} UDPClient_St;

// ────────────────────────────────────────────────
// Globals
// ────────────────────────────────────────────────

static UDPClient_St clients[MAX_CLIENTS] = {0};
static int          masterSocket = -1;  ///< Main server listening socket

// Active game module (lobby or mini-game)
static GameServerInterface_St* activeModule = NULL;
static void*                   activeGameState = NULL;

// ────────────────────────────────────────────────
// External module interfaces (defined in their .c files)
// ────────────────────────────────────────────────

extern GameServerInterface_St lobbyServerInterface;
extern GameServerInterface_St bingoServerInterface;
extern GameServerInterface_St kingServerInterface;

static GameServerInterface_St* gameInterfaces[__miniGameCount] = {
    [MINI_GAME_LOBBY] = &lobbyServerInterface,
    [MINI_GAME_KFF] = &kingServerInterface,
    [MINI_GAME_BINGO] = &bingoServerInterface,
};

// ────────────────────────────────────────────────
// Helper functions
// ────────────────────────────────────────────────

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
            clients[i].active = true;
            clients[i].address = *addr;
            rudpInitConnection(&clients[i].rudpState);
            gettimeofday(&clients[i].lastSeen, NULL);

            log_info("Client slot %d allocated for %s:%d",
                     i, inet_ntoa(addr->sin_addr), ntohs(addr->sin_port));
            return i;
        }
    }

    log_warn("Server is full - rejected new client");
    return -1;
}

/**
    @brief Broadcasts or unicasts a message to clients.
    @param roomId -1 = unicast, >=0 = broadcast (exclude excludeId)
    @param excludeId Client to exclude (broadcast) or target (unicast)
    @param action Action code
    @param payload Message payload
    @param len Payload length
*/
static void serverBroadcast(int roomId, int excludeId, u8 action, const void* payload, u16 len) {
    u8 buffer[2048];
    if (len > MAX_PAYLOAD_SIZE) len = MAX_PAYLOAD_SIZE;

    for (int i = 0; i < MAX_CLIENTS; ++i) {
        bool shouldSend = false;

        if (roomId == UNICAST) {
            // Unicast to specific client
            if (i == excludeId) shouldSend = true;
        } else {
            // Broadcast to everyone except excludeId
            if (clients[i].active && i != excludeId) shouldSend = true;
        }

        if (shouldSend) {
            RUDPHeader_St header;
            rudpGenerateHeader(&clients[i].rudpState, action, &header);
            header.sender_id = htons((u16)excludeId);  // Note: using excludeId as sender for now

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
    RUDPHeader_St response;
    RUDPConnection_St tempConn;
    rudpInitConnection(&tempConn);

    rudpGenerateHeader(&tempConn, ACTION_CODE_LOBBY_ROOM_INFO, &response);
    response.sender_id = htons(999);

    u8 buffer[512];
    memcpy(buffer, &response, sizeof(RUDPHeader_St));

    char info[256];
    const char* moduleName = (activeModule && activeModule->game_name)
                           ? activeModule->game_name : "Unknown";

    snprintf(info, sizeof(info), "%s [%s]", SERVER_DISPLAY_NAME, moduleName);
    memcpy(buffer + sizeof(RUDPHeader_St), info, strlen(info) + 1);

    sendto(masterSocket, buffer, sizeof(RUDPHeader_St) + strlen(info) + 1, 0,
           (struct sockaddr*)clientAddr, sizeof(struct sockaddr_in));
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
                (now.tv_sec - clients[i].lastSeen.tv_sec) * MACROSECONDS_IN_A_SECOND 
              + (now.tv_usec - clients[i].lastSeen.tv_usec);

            if (elapsed > CLIENT_TIMEOUT_US) {
                log_info("Client %d timed out", i);
                clients[i].active = false;

                // Notify others
                serverBroadcast(UNICAST, i, ACTION_CODE_QUIT_GAME, NULL, 0);

                if (activeModule && activeModule->on_player_leave) {
                    activeModule->on_player_leave(activeGameState, i);
                }
            }
        }
    }
}

// ────────────────────────────────────────────────
// Main
// ────────────────────────────────────────────────

/**
    @brief Server program entry point.
    @return 0 on normal exit
*/
int main(void) {
    struct sockaddr_in serverAddr;

    // Initialize clients
    for (int i = 0; i < MAX_CLIENTS; ++i) {
        clients[i].active = false;
    }

    // Create socket
    if ((masterSocket = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Socket creation failed");
        exit(1);
    }

    // Bind socket
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(SERVER_PORT);

    if (bind(masterSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        perror("Bind failed");
        exit(1);
    }

    // Start with lobby as active module
    activeModule = &lobbyServerInterface;

    if (activeModule && activeModule->create_instance) {
        activeGameState = activeModule->create_instance();
    } else {
        log_error("Active module does not implement create_instance");
        exit(1);
    }

    log_info("[SERVER %s] RUDP SERVER STARTED ON PORT %d", 
        getPreciseTimeString(), SERVER_PORT
    );

    log_info("[SERVER %s] ACTIVE MODULE: %s",
        getPreciseTimeString(), activeModule->game_name ? activeModule->game_name : "NULL"
    );

    while (1) {
        fd_set readfds;
        FD_ZERO(&readfds);
        FD_SET(masterSocket, &readfds);

        struct timeval tv = {0, 16666};  // ~60 Hz tick
        int activity = select(masterSocket + 1, &readfds, NULL, NULL, &tv);

        if (activity > 0 && FD_ISSET(masterSocket, &readfds)) {
            struct sockaddr_in clientAddr;
            socklen_t addrLen = sizeof(clientAddr);
            u8 buffer[2048];

            ssize_t received = recvfrom(
                masterSocket, buffer, sizeof(buffer), 0,
                (struct sockaddr*)&clientAddr, &addrLen
            );

            if (received >= (ssize_t) sizeof(RUDPHeader_St)) {
                RUDPHeader_St header;
                memcpy(&header, buffer, sizeof(RUDPHeader_St));

                if (header.action == ACTION_CODE_LOBBY_ROOM_QUERY) {
                    handleDiscoveryQuery(&clientAddr);
                } else {
                    int clientId = findOrCreateClient(&clientAddr);

                    if (clientId == -1 || !rudpProcessIncoming(&clients[clientId].rudpState, &header)) {
                        log_warn("Invalid client id OR invalid package");
                        continue;
                    }

                    gettimeofday(&clients[clientId].lastSeen, NULL);

                    if (header.action == ACTION_CODE_LOBBY_SWITCH_GAME) {
                        MiniGame_Et targetGameId = buffer[sizeof(RUDPHeader_St)];
                        log_info(
                            "[SERVER %s] Client %d requested switch to game ID: %d", 
                            getPreciseTimeString(), clientId, targetGameId
                        );

                        GameServerInterface_St* interface = gameInterfaces[targetGameId];
                        if (interface == NULL) {
                            log_error("Received non-integrated game id");
                            continue;
                        }

                        if (activeModule != interface) {
                            if (activeModule && activeModule->destroy_instance) {
                                    activeModule->destroy_instance(activeGameState);
                                }

                            activeModule = interface;
                            activeGameState = activeModule->create_instance();

                            log_info(
                                "[SERVER %s] Game switch requested for %s",
                                getPreciseTimeString(), interface->game_name
                            );

                            // Confirm switch to requesting client (unicast)
                            u8 switchPayload = targetGameId;
                            serverBroadcast(
                                UNICAST, clientId, ACTION_CODE_LOBBY_SWITCH_GAME, 
                                &switchPayload, 1
                            );
                        }
                    } else if (header.action == ACTION_CODE_LOBBY_CHAT) {
                        serverBroadcast(0, clientId, ACTION_CODE_LOBBY_CHAT,
                                        buffer + sizeof(RUDPHeader_St),
                                        received - sizeof(RUDPHeader_St));

                    // Sink for the every other mini-game actions
                    } else if (activeModule && activeModule->on_action && activeGameState) {
                        activeModule->on_action(
                            activeGameState,
                            clientId,
                            header.action,
                            buffer + sizeof(RUDPHeader_St),
                            received - sizeof(RUDPHeader_St),
                            serverBroadcast
                        );
                    }
                }
            }
        }

        checkTimeouts();

        if (activeModule && activeModule->on_tick) {
            activeModule->on_tick(activeGameState);
        }
    }

    // Cleanup (unreachable in current loop)
    if (activeModule && activeModule->destroy_instance) {
        activeModule->destroy_instance(activeGameState);
    }

    close(masterSocket);
    return 0;
}

#define LOGGER_IMPLEMENTATION
#include "logger.h"