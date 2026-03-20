/**
    @file main.c
    @author Fshimi Hawlk
    @author LeandreB8
    @author i-Charlys (CAILLON Charles)
    @date 2026-02-08
    @date 2026-03-18
    @brief Program entry point for the lobby client – lobby main loop, game scene manager, networking and module dispatching.
    
    Contributors:
        - LeandreB8:
            - Implemented basic lobby's logic (initialization, game loop, ...)
        - Fshimi-Hawlk:
            - Moved & reworked lobby's initialization, game loop and freeing logic in dedicated `lobbyAPI` files
            - Implememted sub-game playablity inside lobby logic via API
            - Added documentation
    
    This file contains the top-level application loop.
    It initializes the window and shared resources, runs the lobby,
    and switches to individual games when triggered (e.g. collision with zone).
    
    Games are loaded on demand via their API (e.g. tetrisAPI.h) and run
    in the same process/window. No separate executables are spawned.
*/

#include "APIs/generalAPI.h"
#include "core/game.h"              // GameScene_Et, general game types

#include "networkInterface.h"
#include "ui/connection_screen.h"

#include "lobbyAPI.h"
// #include "APIs/kingForFourAPI.h"
#include "APIs/bingoAPI.h"
#include "utils/userTypes.h"


enum {
    ACTION_CODE_LOBBY_MOVE = firstAvailableActionCode,
    ACTION_CODE_LOBBY_ROOM_QUERY,
    ACTION_CODE_LOBBY_ROOM_INFO,
    ACTION_CODE_LOBBY_CHAT,
    ACTION_CODE_LOBBY_SWITCH_GAME
};

Error_Et switchMinigame(LobbyGame_St* const game, const MiniGame_Et nextMiniGame) {
    if (nextMiniGame >= __miniGameCount) {
        // some wanring log
        return ERROR_INVALID_ENUM_VAL;
    }

    Error_Et error = OK;

    BaseGame_St* base = &game->miniGameManager.miniGames[nextMiniGame];

    switch (nextMiniGame) {
        case MINI_GAME_LOBBY: {
            if (game->miniGameManager.currentMiniGame == MINI_GAME_LOBBY) break;

            // free the ressource of the last loaded game
            BaseGame_St* base = &game->miniGameManager.miniGames[game->miniGameManager.currentMiniGame];
            base->freeGame(&base);
        } break;

        case MINI_GAME_BATTLESHIP: {

        } break;

        case MINI_GAME_BINGO: {
            BingoGame_St** bingoRef = (BingoGame_St**) base;
            error = bingo_initGame(bingoRef);

            if (error != OK) {
                log_fatal("Bingo initialization failed: error %d", error);
                bingo_freeGame(bingoRef); // in case something was allocated
            }
        } break;

        case MINI_GAME_CONNECT_4: {

        } break;

        case MINI_GAME_KFF: {

        } break;

        case MINI_GAME_MINIGOLF: {

        } break;

        case MINI_GAME_MORPION: {

        } break;

        case MINI_GAME_OTHELLO: {

        } break;

        default: {
            log_error("MiniGame_Et");
            error = ERROR_INVALID_ENUM_VAL;
        }

    }

    game->miniGameManager.currentMiniGame = error == OK 
                                         ? nextMiniGame 
                                         : MINI_GAME_LOBBY;

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
 * @brief Initializes the network connection to a target IP.
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
        log_error("Couldn't connect to %d:%d", targetIp, SERVER_PORT);
    }
    
    rudpInitConnection(&serverConnection);
    
    GameTLVHeader_St tlv = { .game_id = MINI_GAME_LOBBY, .action = ACTION_CODE_JOIN_GAME, .length = 0 };
    u8 buffer[sizeof(RUDPHeader_St) + sizeof(GameTLVHeader_St)];
    
    RUDPHeader_St h; rudpGenerateHeader(&serverConnection, ACTION_GAME_DATA, &h);
    
    memcpy(buffer, &h, sizeof(h)); memcpy(buffer + sizeof(h), &tlv, sizeof(tlv));
    send(networkSocket, buffer, sizeof(buffer), 0);
}

/**
 * @brief Receives and processes incoming network data packets.
 */
void receiveNetworkData(LobbyGame_St* const game) {
    if (networkSocket == -1) return;
    u8 buffer[2048];
    struct sockaddr_in from;
    socklen_t len = sizeof(from);
    
    while (1) {
        ssize_t r = recvfrom(networkSocket, buffer, sizeof(buffer), 0, (struct sockaddr*) &from, &len);
        if (r < (ssize_t) sizeof(RUDPHeader_St)) break;

        RUDPHeader_St* h = (RUDPHeader_St*) buffer;

        if (h->action == ACTION_CODE_LOBBY_ROOM_INFO) {
            addDiscoveredRoom(inet_ntoa(from.sin_addr), (char*) (buffer + sizeof(RUDPHeader_St)));
            continue;
        }

        if (h->action == ACTION_CODE_LOBBY_SWITCH_GAME && rudpProcessIncoming(&serverConnection, h)) {
            u8 targetGameId = *(u8*) (buffer + sizeof(RUDPHeader_St));
            printf("[SYSTEM] Switching to game ID: %d\n", targetGameId);
            switchMinigame(game, targetGameId);
            continue;
        }

        if (h->action == ACTION_GAME_DATA && rudpProcessIncoming(&serverConnection, h)) {
            GameTLVHeader_St* g = (GameTLVHeader_St*) (buffer + sizeof(RUDPHeader_St));
            void* payload = (u8*) g + sizeof(GameTLVHeader_St);
            
            if (game->miniGameManager.miniGameInterfaces[g->game_id]) {
                game->miniGameManager
                    .miniGameInterfaces[g->game_id]
                        ->on_data(
                            ntohs(h->sender_id),
                            g->action, payload,
                            g->length
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
    LobbyGame_St* game = NULL;
    if (lobby_initGame(&game) != OK) {
        log_fatal("Couldn't load the lobby properly.");
        return 1;
    }

    initConnectionScreen();
    
    memset(game->otherPlayers, 0, sizeof(game->otherPlayers));

    // ── Main loop ────────────────────────────────────────────────────────────
    while (!WindowShouldClose()) {
        float dt = GetFrameTime();
        if (dt > 0.1f) dt = 0.1f;
        receiveNetworkData(game);

        static bool switch_sent = false;

        switch (game->currentState) {
            case GAME_STATE_CONNECTION: {
                static float timer = 0; timer += dt;
                if (timer > 2.0f) { discoverServers(); timer = 0; }
                if (updateConnectionScreen()) {
                    initNetwork(getEnteredIP());
                    game->currentState = GAME_STATE_GAMEPLAY;
                }

                BeginDrawing(); {
                    ClearBackground(RAYWHITE);
                    drawConnectionScreen(); 
                } EndDrawing();
            } break;

            case GAME_STATE_GAMEPLAY: {
                if (game->miniGameManager.currentMiniGame == MINI_GAME_LOBBY) {
                    bool trigger = (checkGameTrigger(&game->player) == 1) || IsKeyPressed(KEY_K);
                    if (trigger && !switch_sent) {
                        RUDPHeader_St leave_h; rudpGenerateHeader(&serverConnection, ACTION_CODE_QUIT_GAME, &leave_h);
                        send(networkSocket, &leave_h, sizeof(leave_h), 0);

                        u8 target_id = 1;
                        RUDPHeader_St h; rudpGenerateHeader(&serverConnection, ACTION_CODE_LOBBY_SWITCH_GAME, &h);
                        
                        u8 buffer[sizeof(RUDPHeader_St) + 1];
                        memcpy(buffer, &h, sizeof(h));
                        buffer[sizeof(h)] = target_id;
                        
                        send(networkSocket, buffer, sizeof(buffer), 0);
                        switch_sent = true;
                        printf("[SYSTEM] Requête de switch vers ID %d envoyée.\n", target_id);
                    }
                    if (!trigger) switch_sent = false;
                }

                if (game->miniGameManager.miniGameInterfaces[game->miniGameManager.currentMiniGame]) {
                    game->miniGameManager.miniGameInterfaces[game->miniGameManager.currentMiniGame]->update(dt);

                    BeginDrawing(); {
                        ClearBackground(RAYWHITE);
                        game->miniGameManager.miniGameInterfaces[game->miniGameManager.currentMiniGame]->draw();
                    } EndDrawing();
                }
            } break;
        }
    }

    // ── Cleanup ──────────────────────────────────────────────────────────────
    lobby_freeGame(&game);

    return 0;
}

#define LOGGER_IMPLEMENTATION
#include "logger.h"

#define SYSTEM_SETTINGS_IMPLEMENTATION
#include "systemSettings.h"
