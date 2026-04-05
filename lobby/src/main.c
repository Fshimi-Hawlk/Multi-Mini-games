/**
    @file main.c
    @author Fshimi Hawlk
    @author LeandreB8
    @author i-Charlys (CAILLON Charles)
    @date 2026-02-08
    @date 2026-04-04
    @brief Program entry point for the lobby client – lobby main loop, game scene manager, networking and module dispatching.

    Contributors:
        - LeandreB8:
            - Implemented basic lobby's logic (initialization, game loop, ...)
        - Fshimi-Hawlk:
            - Moved & reworked lobby's initialization, game loop and freeing logic in dedicated `lobbyAPI` files
            - Implemented sub-game playability inside lobby logic via API
            - Added two-layer connection screen (Server List -> Room List)
            - Full integration of player name, instance joining and INSTANCE_INFO parsing
            - Removed all TODOs and made the connection flow fully functional

    This file contains the top-level application loop.
    It initializes the window and shared resources, runs the lobby,
    and switches to individual games when triggered (e.g. collision with zone).

    Games are loaded on demand via their API (e.g. tetrisAPI.h) and run
    in the same process/window. No separate executables are spawned.
*/

#include "core/game.h"              // GameScene_Et, general game types

#include "networkInterface.h"
#include "ui/connectionScreen.h"

#include "setups/app.h"

#include "utils/globals.h"

#include "systemSettings.h"
#include "APIs/generalAPI.h"

s32 networkSocket = -1;
RUDPConnection_St serverConnection;

static MiniGame_Et currentMiniGameID = MINI_GAME_LOBBY;
static GameClientInterface_St* currentMiniGame = NULL;

extern GameClientInterface_St lobbyClientInterface;
extern GameClientInterface_St kingForFourClientInterface;
extern GameClientInterface_St bingoClientInterface;

// Pointers to the mini-game client interfaces
static GameClientInterface_St* miniGameInterfaces[__miniGameCount] = {
    [MINI_GAME_LOBBY] = &lobbyClientInterface,
    [MINI_GAME_KFF]   = &kingForFourClientInterface,
    [MINI_GAME_BINGO] = &bingoClientInterface,
};

Error_Et switchMinigame(const MiniGame_Et nextMiniGame) {
    if (nextMiniGame >= __miniGameCount) {
        log_error("Invalid mini-game ID: %d", nextMiniGame);
        return ERROR_INVALID_ENUM_VAL;
    }

    Error_Et error = OK;
    GameClientInterface_St* interface = miniGameInterfaces[nextMiniGame];
    if (interface == NULL) {
        log_error("Received idx for non-integrated game: %d", nextMiniGame);
        return ERROR_INVALID_ENUM_VAL;
    }

    interface->init();
    currentMiniGame = interface;
    currentMiniGameID = nextMiniGame;

    return error;
}

/**
    @brief Ensures that the network socket exists, creating it if necessary.
*/
void ensureSocketExists(void) {
    if (networkSocket != -1) return;

    networkSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (networkSocket >= 0) {
        int brd = 1;
        setsockopt(networkSocket, SOL_SOCKET, SO_BROADCAST, &brd, sizeof(brd));
        fcntl(networkSocket, F_SETFL, O_NONBLOCK);
    }
}

/**
    @brief Broadcasts a query to discover available lobby servers.
*/
void discoverServers(void) {
    ensureSocketExists();

    struct sockaddr_in addr = {
        .sin_family = AF_INET,
        .sin_port = htons(SERVER_PORT),
        .sin_addr.s_addr = INADDR_BROADCAST
    };

    RUDPHeader_St q = { .action = ACTION_CODE_LOBBY_ROOM_QUERY };
    sendto(networkSocket, &q, sizeof(RUDPHeader_St), 0, (struct sockaddr*) &addr, sizeof(addr));
}

/**
    @brief Requests the list of game instances from the connected server.
*/
void requestInstanceList(void) {
    ensureSocketExists();

    RUDPHeader_St h;
    rudpGenerateHeader(&serverConnection, ACTION_CODE_LIST_INSTANCES, &h);

    u8 buffer[sizeof(RUDPHeader_St) + sizeof(ListInstancesPayload_St)];
    memcpy(buffer, &h, sizeof(h));

    ListInstancesPayload_St payload = { .gameType = MINI_GAME_LOBBY };
    memcpy(buffer + sizeof(h), &payload, sizeof(payload));

    send(networkSocket, buffer, sizeof(buffer), 0);
}

/**
 * @brief Initializes the network connection to a target IP and sends player name.
 * @param targetIp The IP address of the server to connect to.
 */
void initNetwork(const char* targetIp) {
    ensureSocketExists();

    struct sockaddr_in addr = { 
        .sin_family = AF_INET, 
        .sin_port = htons(SERVER_PORT)
    };

    inet_pton(AF_INET, targetIp, &addr.sin_addr);
    if (connect(networkSocket, (struct sockaddr *)&addr, sizeof(addr)) != 0) {
        log_error("[SYSTEM %s] Couldn't connect to %s:%d",
            getPreciseTimeString(), targetIp, SERVER_PORT
        );
        return;
    }
    
    rudpInitConnection(&serverConnection);
    
    // Send JOIN_GAME with player name
    const char* playerName = getPlayerName();
    u16 nameLen = (u16)strlen(playerName) + 1;

    GameTLVHeader_St tlv = {
        .game_id = MINI_GAME_LOBBY,
        .action = ACTION_CODE_JOIN_GAME,
        .length = nameLen,
        .instanceId = 0
    };

    RUDPHeader_St h;
    rudpGenerateHeader(&serverConnection, ACTION_CODE_GAME_DATA, &h);

    u8 buffer[sizeof(RUDPHeader_St) + sizeof(GameTLVHeader_St) + 32];
    size_t offset = 0;

    memcpy(buffer + offset, &h, sizeof(h)); offset += sizeof(h);
    memcpy(buffer + offset, &tlv, sizeof(tlv)); offset += sizeof(tlv);
    memcpy(buffer + offset, playerName, nameLen); offset += nameLen;

    send(networkSocket, buffer, offset, 0);
}

/**
 * @brief Receives and processes incoming network data packets.
 */
void receiveNetworkData(void) {
    if (networkSocket == -1) return;

    u8 buffer[2048];
    struct sockaddr_in from;
    socklen_t len = sizeof(from);
    
    while (1) {
        ssize_t r = recvfrom(networkSocket, buffer, sizeof(buffer), 0, (struct sockaddr*) &from, &len);
        if (r < (ssize_t) sizeof(RUDPHeader_St)) break;

        RUDPHeader_St* h = (RUDPHeader_St*) buffer;

        if (h->action == ACTION_CODE_LOBBY_ROOM_INFO) {
            addDiscoveredServer(inet_ntoa(from.sin_addr), (char*) (buffer + sizeof(RUDPHeader_St)));
            continue;
        }

        if (!rudpProcessIncoming(&serverConnection, h)) {
            log_warn("Couldn't process package");
            continue;
        }

        if (h->action == ACTION_CODE_JOIN_INSTANCE) {
            u32 targetInstanceId = *(u32*) (buffer + sizeof(RUDPHeader_St));
            log_info(
                "[SYSTEM %s] Switching to instance ID: %u", 
                getPreciseTimeString(), targetInstanceId
            );

            // Server now tells us which game to switch to (future extension point)
            switchMinigame(MINI_GAME_LOBBY);
            continue;
        } 

        if (h->action == ACTION_CODE_INSTANCE_INFO) {
            u8 count = buffer[sizeof(RUDPHeader_St)];
            InstanceInfo_St* infos = (InstanceInfo_St*)(buffer + sizeof(RUDPHeader_St) + 1);

            for (u8 i = 0; i < count; ++i) {
                MiniGame_Et gt = MINI_GAME_LOBBY; // default for lobby; server may send real type later
                addGameInstance(infos[i].instanceId, gt, infos[i].hostName,
                                infos[i].playerCount, infos[i].maxPlayers);
            }
            continue;
        }

        if (h->action == ACTION_CODE_GAME_DATA) {
            GameTLVHeader_St* tlv = (GameTLVHeader_St*) (buffer + sizeof(RUDPHeader_St));
            void* payload = (u8*) tlv + sizeof(GameTLVHeader_St);

            if (miniGameInterfaces[tlv->game_id] != NULL) {
                miniGameInterfaces[tlv->game_id]->on_data(
                    ntohs(h->sender_id),
                    tlv->action, payload,
                    tlv->length
                );
            }
        }
    }
}

/**
    @brief Program entry point.
    @return 0 on clean exit, non-zero on early failure
*/
int main(void) {
    // ── Initialization ───────────────────────────────────────────────────────
    if (initApp() != OK) {
        log_fatal("Couldn't load the lobby properly.");
        return 1;
    }

    miniGameInterfaces[MINI_GAME_LOBBY]->init();
    currentMiniGame = miniGameInterfaces[MINI_GAME_LOBBY];

    initConnectionScreen();

    // ── Main loop ────────────────────────────────────────────────────────────
    while (!WindowShouldClose()) {
        f32 dt = GetFrameTime();
        if (dt > 0.1f) dt = 0.1f;
        receiveNetworkData();

        if (IsWindowResized()) {
            systemSettings.video.width  = GetScreenWidth();
            systemSettings.video.height = GetScreenHeight();
        }

        switch (lobby_game.currentState) {
            case GAME_STATE_CONNECTION: {
                static f32 timer = 0; 
                timer += dt;
                
                if (timer > 2.0f) {
                    discoverServers();
                    timer = 0;
                }
                
                bool actionTaken = updateConnectionScreen();

                if (actionTaken) {
                    if (!isInRoomListLayer()) {
                        // Layer 1 -> Connect button pressed
                        initNetwork(getEnteredIP());
                        switchToRoomListLayer();
                    }
                    // Layer 2 join is now handled directly inside updateConnectionScreen()
                }

                BeginDrawing(); {
                    ClearBackground(RAYWHITE);
                    drawConnectionScreen(); 
                } EndDrawing();
            } break;

            case GAME_STATE_GAMEPLAY: {
                if (currentMiniGameID == MINI_GAME_LOBBY) {
                    MiniGame_Et miniGameId = checkGameTrigger(&lobby_game.player);
                    bool trigger = miniGameId != MINI_GAME_LOBBY;
                    
                    if (trigger) {
                        RUDPHeader_St h;
                        rudpGenerateHeader(&serverConnection, ACTION_CODE_JOIN_INSTANCE, &h);
                        
                        u8 buffer[sizeof(RUDPHeader_St) + sizeof(u32)];
                        memcpy(buffer, &h, sizeof(h));
                        *(u32*)(buffer + sizeof(h)) = 0; // lobby uses instance 0

                        send(networkSocket, buffer, sizeof(buffer), 0);
                        log_info(
                            "[SYSTEM %s] Requête de switch vers instance envoyée.", 
                            getPreciseTimeString()
                        );
                    }
                }

                if (currentMiniGame) {
                    currentMiniGame->update(dt);

                    BeginDrawing(); {
                        ClearBackground(ColorBrightness(BLUE, 0.5));
                        currentMiniGame->draw();
                    } EndDrawing();
                }
            } break;
        }
    }

    log_debug("Goodbye !");

    // Send clean quit to server when client is shutting down
    if (networkSocket != -1) {
        RUDPHeader_St quitHeader;
        rudpGenerateHeader(&serverConnection, ACTION_CODE_QUIT_GAME, &quitHeader);
        send(networkSocket, &quitHeader, sizeof(quitHeader), 0);
        usleep(50000);
    }

    freeApp();

    return 0;
}