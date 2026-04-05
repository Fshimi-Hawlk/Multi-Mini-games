/**
    @file server.c
    @author i-Charlys (CAILLON Charles)
    @author Fshimi-Hawlk
    @date 2026-03-18
    @date 2026-04-04
    @brief Authoritative RUDP server with full multi-instance support.

    Manages multiple client connections and routes every packet to the exact
    GameInstance the client belongs to. The lobby is a permanent instance (ID 0).
    Supports any number of independent instances of any mini-game simultaneously.
    All broadcasting is strictly scoped to players inside the same instance.

    Contributors:
        - i-Charlys (CAILLON Charles):
            - Original single-instance server
        - Fshimi-Hawlk:
            - Full multi-instance architecture (GameInstance_St, scoped broadcasting,
              instance creation/joining, player name support)

    Manages multiple client connections, routes packets to the correct GameInstance_St
    (lobby or any mini-game room). The lobby is a permanent instance (ID 0).
    All packets are strictly routed to the instance the sending client belongs to.
    Supports any number of independent instances of any mini-game simultaneously.

    Communication diagram (multi-instance):
    [CLIENT 1] <----RUDP----> [ SERVER ] <----RUDP----> [CLIENT 2]
                                  |
                                  v
                       +---------------------+
                       |   GameInstance 0    |  ← Lobby (permanent)
                       +---------------------+
                       |   GameInstance N    |  ← e.g. Bingo room #3
                       |   GameInstance M    |  ← e.g. King-for-Four room #7
                       +---------------------+

    Packet flow:
        - ACTION_CODE_CREATE_INSTANCE → findOrCreateInstance() + move player
        - ACTION_CODE_GAME_DATA       → routed to the client's current instance
        - ACTION_CODE_QUIT_GAME       → handlePlayerLeave() + possible instance cleanup
        - Discovery / room list queries are handled separately

    @note Currently supports lobby, King-for-Four and Bingo.
          Easy to extend for more modules.
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
#define CLIENT_TIMEOUT_US       (3 * 60 * MACROSECONDS_IN_A_SECOND)
#define SERVER_DISPLAY_NAME     "Multi-Mini-Games Server"
#define MAX_PAYLOAD_SIZE        (2048 - sizeof(RUDPHeader_St))
#define MAX_INSTANCES           32

RUDPConnection_St serverConnection;

// ────────────────────────────────────────────────
// Action codes (lobby-specific)
// ────────────────────────────────────────────────

enum {
    ACTION_CODE_LOBBY_MOVE          = firstAvailableActionCode,
    ACTION_CODE_LOBBY_ROOM_QUERY,
    ACTION_CODE_LOBBY_ROOM_INFO,
    ACTION_CODE_LOBBY_CHAT,
};

// ────────────────────────────────────────────────
// Internal client representation
// ────────────────────────────────────────────────

/**
    @brief Server-side representation of a connected client.
*/
typedef struct {
    bool               active;              ///< True when slot is occupied
    struct sockaddr_in address;             ///< Client UDP endpoint
    RUDPConnection_St  rudpState;           ///< Per-client RUDP state
    struct timeval     lastSeen;            ///< Last activity timestamp
    char               playerName[32];      ///< Player-chosen name (sent on join)
    u32                currentInstanceId;   ///< Which GameInstance the client is in (0 = lobby)
} UDPClient_St;

// ────────────────────────────────────────────────
// Globals
// ────────────────────────────────────────────────

static UDPClient_St clients[MAX_CLIENTS] = {0};
static s32          masterSocket = -1;  ///< Main server listening socket

static GameInstance_St* instances[MAX_INSTANCES] = {0};
static u32              nextInstanceId = 1;   // 0 is reserved for the permanent lobby

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
    @brief Finds an existing GameInstance or creates a new one.

    Special case: MINI_GAME_LOBBY always returns the permanent instance 0.
    For other games a new instance is allocated if a free slot exists.

    @param gameType      Type of game to create/find
    @param hostClientId  Client ID of the creator (or -1 for lobby)
    @return Pointer to the GameInstance_St or NULL if no slot available
*/
static GameInstance_St* findOrCreateInstance(MiniGame_Et gameType, s32 hostClientId) {
    // Special case: lobby is always instance 0
    if (gameType == MINI_GAME_LOBBY) {
        if (instances[0] == NULL) {
            instances[0] = calloc(1, sizeof(GameInstance_St));
            instances[0]->id          = 0;
            instances[0]->gameType    = MINI_GAME_LOBBY;
            instances[0]->interface   = &lobbyServerInterface;
            instances[0]->gameState   = lobbyServerInterface.createInstance();
            instances[0]->active      = true;
            instances[0]->hostClientId = -1;
            log_info("Permanent lobby instance created");
        }

        return instances[0];
    }


    // Look for existing instance
    for (s32 i = 1; i < MAX_INSTANCES; ++i) {
        if (instances[i]->gameType == gameType && instances[i]->hostClientId == hostClientId)
            return instances[i];
    }

    // Look for free slot
    for (s32 i = 1; i < MAX_INSTANCES; ++i) {
        if (instances[i] == NULL) {
            GameInstance_St* inst = calloc(1, sizeof(GameInstance_St));
            inst->id              = nextInstanceId++;
            inst->gameType        = gameType;
            inst->interface       = gameInterfaces[gameType];
            inst->gameState       = inst->interface->createInstance();
            inst->active          = true;
            inst->hostClientId    = hostClientId;
            instances[i]          = inst;

            log_info("Created new instance %u of game '%s' (host client %d)",
                     inst->id, inst->interface->game_name, hostClientId);
            return inst;
        }
    }
    
    log_warn("No free instance slots");
    return NULL;
}

/**
    @brief Finds existing client by address or creates a new slot.
    @param addr Client address
    @return Client index (0..MAX_CLIENTS-1) or -1 if server is full
*/
static s32 findOrCreateClient(struct sockaddr_in* addr) {
    // Look for existing client
    for (s32 i = 0; i < MAX_CLIENTS; ++i) {
        if (clients[i].active &&
            clients[i].address.sin_addr.s_addr == addr->sin_addr.s_addr &&
            clients[i].address.sin_port == addr->sin_port) {
            return i;
        }
    }

    // Create new client
    for (s32 i = 0; i < MAX_CLIENTS; ++i) {
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
    @brief Returns the GameInstance the given client currently belongs to.
    @param clientId Valid client index
    @return Pointer to GameInstance_St or NULL if client not found or inactive
*/
static GameInstance_St* getClientInstance(s32 clientId) {
    if (clientId < 0 || clientId >= MAX_CLIENTS || !clients[clientId].active) return NULL;
    u32 iid = clients[clientId].currentInstanceId;

    for (s32 i = 0; i < MAX_INSTANCES; ++i) {
        if (instances[i] && instances[i]->id == iid) return instances[i];
    }
    return NULL;
}

/**
    @brief Broadcasts a message **only to players inside the given instance**.
    @param instance   Target GameInstance (must not be NULL)
    @param excludeId  Client to exclude from broadcast (or -1 for none)
    @param action     Action code
    @param payload    Message payload
    @param len        Payload length in bytes
*/
static void instanceBroadcast(GameInstance_St* instance, s32 excludeId, u8 action, const void* payload, u16 len) {
    if (instance == NULL || len > MAX_PAYLOAD_SIZE) return;

    u8 buffer[2048];

    for (s32 i = 0; i < instance->playerCount; ++i) {
        s32 clientId = instance->players[i];
        if (clientId == excludeId || clientId < 0 || !clients[clientId].active) continue;

        RUDPHeader_St header;
        rudpGenerateHeader(&clients[clientId].rudpState, action, &header);
        header.sender_id = htons((u16)excludeId);

        memcpy(buffer, &header, sizeof(RUDPHeader_St));
        if (len > 0 && payload != NULL) {
            memcpy(buffer + sizeof(RUDPHeader_St), payload, len);
        }

        sendto(masterSocket, buffer, sizeof(RUDPHeader_St) + len, 0,
               (struct sockaddr*)&clients[clientId].address, sizeof(struct sockaddr_in));
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

    snprintf(info, sizeof(info), "%s", SERVER_DISPLAY_NAME);
    memcpy(buffer + sizeof(RUDPHeader_St), info, strlen(info) + 1);

    sendto(masterSocket, buffer, sizeof(RUDPHeader_St) + strlen(info) + 1, 0,
           (struct sockaddr*)clientAddr, sizeof(struct sockaddr_in));
}

/**
    @brief Called whenever a player leaves (timeout or explicit quit).
           Removes the player from their current instance and destroys the instance
           if it becomes empty and is not the lobby.
*/
static void handlePlayerLeave(s32 clientId) {
    GameInstance_St* inst = getClientInstance(clientId);
    if (inst && inst->interface && inst->interface->onPlayerLeave) {
        inst->interface->onPlayerLeave(inst->gameState, clientId);
    }

    // Remove from instance list
    if (inst) {
        for (s32 i = 0; i < inst->playerCount; ++i) {
            if (inst->players[i] == clientId) {
                inst->players[i] = inst->players[--inst->playerCount];
                break;
            }
        }
        clients[clientId].currentInstanceId = 0; // back to lobby
    }

    // If instance is now empty and not the lobby → destroy it
    if (inst && inst->id != 0 && inst->playerCount == 0) {
        if (inst->interface && inst->interface->destroyInstance) {
            inst->interface->destroyInstance(inst->gameState);
        }

        free(inst);

        // remove from instances array (simple linear search)
        for (s32 i = 0; i < MAX_INSTANCES; ++i) {
            if (instances[i] == inst) { instances[i] = NULL; break; }
        }
        log_info("Destroyed empty instance %u", inst->id);
    }
}

/**
    @brief Checks for timed-out clients and disconnects them.
*/
static void checkTimeouts(void) {
    struct timeval now;
    gettimeofday(&now, NULL);

    for (s32 i = 0; i < MAX_CLIENTS; ++i) {
        if (clients[i].active) {
            long long elapsed = \
                (now.tv_sec - clients[i].lastSeen.tv_sec) * MACROSECONDS_IN_A_SECOND
              + (now.tv_usec - clients[i].lastSeen.tv_usec);

            if (elapsed > CLIENT_TIMEOUT_US) {
                log_info("Client %d timed out", i);
                clients[i].active = false;

                handlePlayerLeave(i);
            }
        }
    }
}

/**
    @brief Sends a list of active instances of a given game type to a client.
    @param clientId   Target client
    @param gameType   Game type to list
*/
static void sendInstanceList(s32 clientId, MiniGame_Et gameType) {
    if (clientId < 0 || clientId >= MAX_CLIENTS || !clients[clientId].active) return;

    InstanceInfo_St infos[MAX_INSTANCES];
    u8 count = 0;

    for (s32 i = 0; i < MAX_INSTANCES && count < MAX_INSTANCES; ++i) {
        if (instances[i] && instances[i]->gameType == gameType) {
            infos[count].instanceId  = instances[i]->id;
            infos[count].playerCount = instances[i]->playerCount;
            infos[count].maxPlayers  = (instances[i]->id == 0) ? 0 : MAX_CLIENTS; // lobby has no limit

            if (instances[i]->hostClientId >= 0 && instances[i]->hostClientId < MAX_CLIENTS) {
                strncpy(infos[count].hostName, clients[instances[i]->hostClientId].playerName, 31);
            } else {
                strncpy(infos[count].hostName, (instances[i]->id == 0) ? "Lobby" : "Unknown", 31);
            }
            infos[count].hostName[31] = '\0';
            ++count;
        }
    }

    // Build response
    u8 buffer[2048];
    RUDPHeader_St header;
    rudpGenerateHeader(&clients[clientId].rudpState, ACTION_CODE_INSTANCE_INFO, &header);
    header.sender_id = htons(999);

    memcpy(buffer, &header, sizeof(RUDPHeader_St));

    // First byte = number of instances, then the array
    buffer[sizeof(RUDPHeader_St)] = count;
    if (count > 0) {
        memcpy(buffer + sizeof(RUDPHeader_St) + 1, infos, count * sizeof(InstanceInfo_St));
    }

    sendto(
        masterSocket, buffer,
        sizeof(RUDPHeader_St) + 1 + count * sizeof(InstanceInfo_St),
        0, 
        (struct sockaddr*)&clients[clientId].address, 
        sizeof(struct sockaddr_in)
    );
}

// ────────────────────────────────────────────────
// Main
// ────────────────────────────────────────────────

/**
    @brief Server program entry point.
    @return 0 on normal exit
*/
s32 main(void) {
    struct sockaddr_in serverAddr;

    // Initialize clients
    for (s32 i = 0; i < MAX_CLIENTS; ++i) {
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

    findOrCreateInstance(MINI_GAME_LOBBY, -1);

    log_info("[SERVER %s] RUDP SERVER STARTED ON PORT %d",
        getPreciseTimeString(), SERVER_PORT
    );

    while (1) {
        fd_set readfds;
        FD_ZERO(&readfds);
        FD_SET(masterSocket, &readfds);

        struct timeval tv = {0, 16666};  // ~60 Hz tick
        s32 activity = select(masterSocket + 1, &readfds, NULL, NULL, &tv);

        if (activity <= 0 || !FD_ISSET(masterSocket, &readfds)) {
            checkTimeouts();

            // Tick every active instance
            for (s32 i = 0; i < MAX_INSTANCES; ++i) {
                if (instances[i] && instances[i]->active && instances[i]->interface->onTick) {
                    instances[i]->interface->onTick(instances[i]->gameState);
                }
            }
            continue;
        }

    
        struct sockaddr_in clientAddr;
        socklen_t addrLen = sizeof(clientAddr);
        u8 buffer[2048];

        ssize_t received = recvfrom(
            masterSocket, buffer, sizeof(buffer), 0,
            (struct sockaddr*)&clientAddr, &addrLen
        );

        if (received < (ssize_t) sizeof(RUDPHeader_St)) {
            continue;
        }

        RUDPHeader_St header;
        memcpy(&header, buffer, sizeof(RUDPHeader_St));

        if (header.action == ACTION_CODE_LOBBY_ROOM_QUERY) {
            handleDiscoveryQuery(&clientAddr);
            continue;
        }

        s32 clientId = findOrCreateClient(&clientAddr);
        if (clientId == -1 || !rudpProcessIncoming(&clients[clientId].rudpState, &header)) {
            log_warn("Invalid client id OR invalid package");
            continue;
        }

        gettimeofday(&clients[clientId].lastSeen, NULL);

        GameInstance_St* inst = getClientInstance(clientId);

        if (header.action == ACTION_CODE_CREATE_INSTANCE) {
            // payload = CreateInstancePayload_St
            MiniGame_Et gt = ((CreateInstancePayload_St*) (buffer + sizeof(RUDPHeader_St)))->gameType;
            GameInstance_St* newInst = findOrCreateInstance(gt, clientId);
            if (newInst) {
                // move player from lobby to new instance

                if (inst) {
                    handlePlayerLeave(clientId); // leave old
                }

                clients[clientId].currentInstanceId = newInst->id;
                newInst->players[newInst->playerCount++] = clientId;
                newInst->interface->onAction(
                    newInst->gameState, clientId,
                    ACTION_CODE_JOIN_GAME, NULL, 0,
                    instanceBroadcast
                );
            }
        } else if (header.action == ACTION_CODE_JOIN_INSTANCE) {
            if (received < (ssize_t)(sizeof(RUDPHeader_St) + sizeof(JoinInstancePayload_St))) {
                log_warn("Malformed JOIN_INSTANCE payload");
                continue;
            }

            u32 targetInstanceId = ((JoinInstancePayload_St*)(buffer + sizeof(RUDPHeader_St)))->instanceId;

            GameInstance_St* targetInst = NULL;
            for (s32 i = 0; i < MAX_INSTANCES; ++i) {
                if (instances[i] && instances[i]->id == targetInstanceId) {
                    targetInst = instances[i];
                    break;
                }
            }

            if (targetInst) {
                if (inst) {
                    handlePlayerLeave(clientId); // leave old instance
                }

                clients[clientId].currentInstanceId = targetInst->id;
                targetInst->players[targetInst->playerCount++] = clientId;
                targetInst->interface->onAction(
                    targetInst->gameState, clientId,
                    ACTION_CODE_JOIN_GAME, NULL, 0,
                    instanceBroadcast
                );

                log_info("Client %d joined instance %u", clientId, targetInstanceId);
            } else {
                log_warn("Client %d tried to join non-existent instance %u", clientId, targetInstanceId);
            }
        } else if (header.action == ACTION_CODE_QUIT_GAME) {
            log_info(
                "[SERVER %s] Client %d sent explicit quit", 
                getPreciseTimeString(), clientId
            );
            
            clients[clientId].active = false;
            handlePlayerLeave(clientId);
        } else if (header.action == ACTION_CODE_LIST_INSTANCES) {
            if (received < (ssize_t)(sizeof(RUDPHeader_St) + sizeof(ListInstancesPayload_St))) {
                log_warn("Malformed LIST_INSTANCES payload");
                continue;
            }
            MiniGame_Et gt = ((ListInstancesPayload_St*)(buffer + sizeof(RUDPHeader_St)))->gameType;
            sendInstanceList(clientId, gt);
        } else if (inst && inst->interface && inst->interface->onAction) {
            // Normal game-specific action – routed to the correct instance
            inst->interface->onAction(
                inst,
                clientId,
                header.action,
                buffer + sizeof(RUDPHeader_St),
                received - sizeof(RUDPHeader_St),
                instanceBroadcast   // scoped to this instance only
            );
        }
    }

    // Cleanup (unreachable in current loop)
    for (s32 i = 0; i < MAX_INSTANCES; ++i) {
        if (instances[i]) {
            if (instances[i]->interface && instances[i]->interface->destroyInstance) {
                instances[i]->interface->destroyInstance(instances[i]->gameState);
            }
            free(instances[i]);
        }
    }

    close(masterSocket);
    return 0;
}