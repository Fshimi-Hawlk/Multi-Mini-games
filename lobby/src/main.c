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
#include "ui/menus.h"
#include "utils/chat.h"
#include "utils/globals.h"
#include "firstparty/progress.h"
#include "firstparty/leaderboard.h"

#include "lobbyAPI.h"
#include "APIs/bingoAPI.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <fcntl.h>

/** @brief Global player progress. */
PlayerProgress_St g_progress;

enum {
    ACTION_CODE_LOBBY_MOVE = firstAvailableActionCode,
    ACTION_CODE_LOBBY_ROOM_QUERY,
    ACTION_CODE_LOBBY_ROOM_INFO,
    ACTION_CODE_LOBBY_CHAT,
    ACTION_CODE_LOBBY_SWITCH_GAME
};

/** @brief Registry of available mini-game modules. */
static MiniGameModule* game_registry[256] = {0};

/** @brief ID of the currently active mini-game module. */
u8 active_game_id = 0;

extern MiniGameModule KingForFourClientModule;

/** @brief Port used for server communication. */
#define SERVER_PORT 8080

/** @brief Global network socket for the client. */
s32 networkSocket = -1;

/** @brief RUDP connection state for the server. */
RUDPConnection_St serverConnection;

/** @brief Assigned player ID from server. */
int my_id = -1;

void register_minigame(MiniGameModule* module) {
    if (module && module->id < 256) {
        game_registry[module->id] = module;
        if (module->init) module->init();
    }
}


Error_Et switchMinigame(LobbyGame_St* const game, const MiniGame_Et nextMiniGame) {
    if (nextMiniGame >= __miniGameCount) {
        // some warning log
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
                bingo_initGame(bingoRef); // in case something was allocated
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
 * @brief Ensures that the network socket exists, creating it if necessary.
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
        log_error("Couldn't connect to %s:%d", targetIp, SERVER_PORT);
    }
    
    rudpInitConnection(&serverConnection);
    
    RUDPHeader_St h; rudpGenerateHeader(&serverConnection, ACTION_CODE_JOIN_GAME, &h);
    send(networkSocket, &h, sizeof(h), 0);
}

/**
 * @brief Receives and processes incoming network data packets.
 * @param game Pointer to the lobby game structure.
 */
void receiveNetworkData(LobbyGame_St* game) {
    if (networkSocket == -1) return;
    u8 buffer[2048];
    struct sockaddr_in from; socklen_t len = sizeof(from);
    
    while (1) {
        ssize_t r = recvfrom(networkSocket, buffer, sizeof(buffer), 0, (struct sockaddr*) &from, &len);
        if (r < (ssize_t) sizeof(RUDPHeader_St)) break;

        RUDPHeader_St* h = (RUDPHeader_St*) buffer;

        if (h->action == ACTION_CODE_LOBBY_ROOM_INFO) {
            addDiscoveredRoom(inet_ntoa(from.sin_addr), (char*) (buffer + sizeof(RUDPHeader_St)));
            continue;
        }

        if (h->action == ACTION_CODE_JOIN_ACK) {
            u16 temp_id;
            memcpy(&temp_id, buffer + sizeof(RUDPHeader_St), sizeof(u16)); 
            my_id = temp_id;
            printf("[SYSTEM] Connecté au lobby. Mon ID: %d\n", my_id);
            game->currentState = GAME_STATE_LOBBY;
            continue;
        }

        int sid = ntohs(h->sender_id);

        if (h->action == ACTION_CODE_LOBBY_SWITCH_GAME && rudpProcessIncoming(&serverConnection, h)) {
            u8 targetGameId = *(u8*) (buffer + sizeof(RUDPHeader_St));
            printf("[SYSTEM] Switching to game ID: %d\n", targetGameId);
            if (targetGameId == 0) {
                active_game_id = 0;
                game->currentState = GAME_STATE_LOBBY;
            } else if (game_registry[targetGameId]) {
                active_game_id = targetGameId;
                game->currentState = GAME_STATE_INGAME;
                printf("[SYSTEM] Switch vers module ID:%d\n", targetGameId);
            }
            continue;
        }

        if (h->action == ACTION_CODE_LOBBY_MOVE) {
            if (sid == my_id) continue;
            if (sid >= 0 && sid < MAX_CLIENTS) {
                Player_St incoming;
                memcpy(&incoming, buffer + sizeof(RUDPHeader_St), sizeof(Player_St));
                
                // If it's the first time we see this player, teleport them
                if (!game->otherPlayers[sid].active) {
                    game->otherPlayers[sid] = incoming;
                    game->otherPlayers[sid].targetPosition = incoming.position;
                } else {
                    // Update all fields EXCEPT current position
                    Vector2 currentPos = game->otherPlayers[sid].position;
                    game->otherPlayers[sid] = incoming;
                    game->otherPlayers[sid].position = currentPos;
                    game->otherPlayers[sid].targetPosition = incoming.position;
                }
                game->otherPlayers[sid].active = true;
            }
            continue;
        }

        if (h->action == ACTION_CODE_QUIT_GAME) {
            if (sid >= 0 && sid < MAX_CLIENTS) game->otherPlayers[sid].active = false;
            continue;
        }

        if (h->action == 5 /* ACTION_GAME_DATA */ && rudpProcessIncoming(&serverConnection, h)) {
            GameTLVHeader_St* g = (GameTLVHeader_St*) (buffer + sizeof(RUDPHeader_St));
            void* payload = (u8*) g + sizeof(GameTLVHeader_St);
            
            // Global chat processing
            if (g->action == ACTION_CODE_LOBBY_CHAT && g->game_id == active_game_id) {
                if (sid != my_id) {
                    const char* sender_name = (sid == 999) ? "SERVEUR" : TextFormat("Joueur %d", sid);
                    AddChatMessage(sender_name, (char*) payload);
                }
            }

            // Dispatch to module
            if (game_registry[g->game_id]) {
                game_registry[g->game_id]->on_data(sid, g->action, payload, g->length);
            }
        }
    }
}

int main(void) {
    g_progress = LoadProgress();
    
    // Update AP tiers based on current leaderboard placements
    for (int g = 1; g < MAX_GAMES_PROGRESS; g++) {
        Leaderboard_St lb = LoadLeaderboard(g);
        g_progress.total_players[g][0][0] = lb.count;
        g_progress.current_ap[g] = CalculateAPTier(g_progress.leaderboard_rank[g][0][0], lb.count);
    }
    
    InitMenus();

    LobbyGame_St* game = NULL;
    if (lobby_initGame(&game) != OK) {
        log_fatal("Couldn't load the lobby properly.");
        return 1;
    }

    InitConnectionScreen();
    
    for (int i = 0; i < MAX_CLIENTS; i++) game->otherPlayers[i].active = false;

    register_minigame(&KingForFourClientModule);

    float move_timer = 0;

    // ── Main loop ────────────────────────────────────────────────────────────
    while (!WindowShouldClose()) {
        float dt = GetFrameTime();
        if (dt > 0.1f) dt = 0.1f;
        receiveNetworkData(game);

        UpdateMenu();
        if (g_currentMenu != MENU_NONE) {
            if (g_currentMenu == MENU_PAUSE && IsKeyPressed(KEY_ESCAPE)) g_currentMenu = MENU_NONE;
        }

        if (g_currentMenu == MENU_NONE) {
            UpdateChat();

            Player_St* local_p = &game->player;

            if (game->currentState == GAME_STATE_LOBBY && active_game_id == 0) {
                // Lobby network sync (send pos) - Throttled at 60Hz
                move_timer += dt;
                static Vector2 lastSent = {0};
                if (move_timer >= 0.016f && (local_p->position.x != lastSent.x || local_p->position.y != lastSent.y)) {
                    RUDPHeader_St h; rudpGenerateHeader(&serverConnection, ACTION_CODE_LOBBY_MOVE, &h);
                    h.sender_id = htons((u16)my_id);
                    u8 buf[1024];
                    memcpy(buf, &h, sizeof(h));
                    memcpy(buf + sizeof(h), local_p, sizeof(Player_St));
                    send(networkSocket, buf, sizeof(h) + sizeof(Player_St), 0);
                    lastSent = local_p->position;
                    move_timer = 0;
                }

                // Game trigger check
                if (checkGameTrigger(local_p)) {
                    DrawText("APPUYEZ SUR ENTRÉE POUR JOUER", GetScreenWidth()/2 - MeasureText("APPUYEZ SUR ENTRÉE POUR JOUER", 20)/2, GetScreenHeight() - 100, 20, GOLD);
                    
                    if (IsKeyPressed(KEY_ENTER)) {
                        u8 target_id = 1; // King For Four
                        RUDPHeader_St h; rudpGenerateHeader(&serverConnection, ACTION_CODE_LOBBY_SWITCH_GAME, &h);
                        h.sender_id = htons((u16)my_id);
                        u8 buf[64];
                        memcpy(buf, &h, sizeof(h));
                        buf[sizeof(h)] = target_id;
                        send(networkSocket, buf, sizeof(h) + 1, 0);
                        game->currentState = GAME_STATE_WAITING_SWITCH; 
                        printf("[SYSTEM] Demande de switch envoyée, attente confirmation...\n");
                    }
                }
            }
        }

        BeginDrawing();
        ClearBackground(RAYWHITE);

        if (g_currentMenu != MENU_NONE && g_currentMenu != MENU_PAUSE) {
            DrawMenu();
        } else {
            switch (game->currentState) {
                case GAME_STATE_DISCONNECTED:
                    if (UpdateConnectionScreen()) {
                        initNetwork(GetEnteredIP());
                        game->currentState = GAME_STATE_CONNECTING;
                    }
                    DrawConnectionScreen();
                    break;

                case GAME_STATE_CONNECTING:
                    DrawText("CONNEXION EN COURS...", 100, 100, 30, GRAY);
                    break;

                case GAME_STATE_LOBBY:
                case GAME_STATE_WAITING_SWITCH:
                    lobby_gameLoop(game);
                    if (game->currentState == GAME_STATE_WAITING_SWITCH) {
                        DrawText("CHARGEMENT DU JEU...", GetScreenWidth()/2 - 100, GetScreenHeight()/2, 20, WHITE);
                    }
                    break;

                case GAME_STATE_INGAME:
                    if (game_registry[active_game_id]) {
                        game_registry[active_game_id]->update(dt);
                        if (game->currentState == GAME_STATE_INGAME && game_registry[active_game_id]) {
                            game_registry[active_game_id]->draw();
                        }
                    }
                    break;
            }

            if (g_currentMenu == MENU_PAUSE) DrawMenu();

            DrawChat();
        }
        
        EndDrawing();
    }

    SaveProgress(&g_progress);
    lobby_freeGame(&game);

    return 0;
}

#define LOGGER_IMPLEMENTATION
#include "logger.h"

#define SYSTEM_SETTINGS_IMPLEMENTATION
#include "systemSettings.h"
