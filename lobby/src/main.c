/**
    @file main.c
    @author Fshimi Hawlk
    @author LeandreB8
    @author i-Charlys
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
#include "core/chat.h"
#include "utils/globals.h"
#include "firstparty/progress.h"
#include "firstparty/leaderboard.h"

#include "systemSettings.h"
#include "lobbyAPI.h"
#include "APIs/bingoAPI.h"
#include "APIs/kingForFourAPI.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <errno.h>

/** @brief Global player progress. */
PlayerProgress_St g_progress;

/** @brief Registry of available mini-game modules. */
static GameClientInterface_St* game_registry[256] = {0};

/** @brief ID of the currently active mini-game module. */
u8 active_game_id = 0;

extern GameClientInterface_St KingForFourClientModule;
extern GameClientInterface_St ChessClientModule;
extern GameClientInterface_St RubikClientModule;

/** @brief Port used for server communication and discovery. */
#define SERVER_PORT 8080
#define DISCOVERY_PORT 8080

/** @brief Global network socket for the client. */
s32 networkSocket = -1;

/** @brief RUDP connection state for the server. */
RUDPConnection_St serverConnection;

/** @brief Assigned player ID from server. */
int my_id = -1;

LobbyGame_St* g_lobbyGame = NULL;

Error_Et switchMinigame(LobbyGame_St* const game, const MiniGame_Et nextMiniGame);

void discover_servers(void) {
    void discoverServers(void);
    discoverServers();
}

void switch_minigame(u8 game_id) {
    if (g_lobbyGame) {
        if (switchMinigame(g_lobbyGame, (MiniGame_Et)game_id) == OK) {
            active_game_id = game_id;
            if (game_id == 0) g_lobbyGame->currentState = GAME_STATE_LOBBY;
            else g_lobbyGame->currentState = GAME_STATE_INGAME;
        }
    }
}

void start_solo_mode(void) {
    if (g_lobbyGame) {
        my_id = 0;
        g_lobbyGame->currentState = GAME_STATE_LOBBY;
        log_info("Mode Solo activé.");
    }
}

static bool server_spawned = false;

void kill_server(void) {
    if (server_spawned) {
        log_info("Arrêt du serveur...");
#ifdef _WIN32
        system("taskkill /F /IM server.exe > nul 2>&1");
#else
        system("killall server 2>/dev/null");
#endif
        server_spawned = false;
    }
}

void force_kill_server(void) {
#ifdef _WIN32
    system("taskkill /F /IM server.exe > nul 2>&1");
#else
    system("killall server 2>/dev/null");
#endif
}

void spawn_server(void) {
    if (server_spawned) return;
    log_info("Lancement du serveur...");
#ifdef _WIN32
    system("start /B reseau\\build\\bin\\server.exe");
#else
    system("./reseau/build/bin/server &");
#endif
    server_spawned = true;
}

void register_minigame(GameClientInterface_St* module) {
    if (module) {
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

    BaseGame_St** base = &game->miniGameManager.miniGames[nextMiniGame];

    switch (nextMiniGame) {
        case MINI_GAME_LOBBY: {
            if (game->miniGameManager.currentMiniGame == MINI_GAME_LOBBY) break;

            // free the ressource of the last loaded game
            BaseGame_St** lastBase = &game->miniGameManager.miniGames[game->miniGameManager.currentMiniGame];
            if (*lastBase && (*lastBase)->freeGame) (*lastBase)->freeGame(lastBase);
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
            KingForFourGame_St** kffRef = (KingForFourGame_St**) base;
            error = kingforfour_initGame__full(kffRef, (KingForFourConfigs_St){.fps = 60});

            if (error != OK) {
                log_fatal("KFF initialization failed: error %d", error);
            }
        } break;

        case MINI_GAME_CHESS: {
            extern Error_Et chess_initGame(void** gameRef);
            error = chess_initGame((void**)base);
        } break;

        case MINI_GAME_CUBE: {
            extern Error_Et rubik_initGame(void** gameRef);
            error = rubik_initGame((void**)base);
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
        .sin_port = htons(DISCOVERY_PORT),
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
    struct sockaddr_in from; socklen_t len;
    
    while (1) {
        len = sizeof(from);
        ssize_t r = recvfrom(networkSocket, buffer, sizeof(buffer), 0, (struct sockaddr*)&from, &len);

        if (r < 0) {
    #ifdef _WIN32
            int err = WSAGetLastError();
            if (err == WSAEWOULDBLOCK || err == WSAECONNRESET) break;
    #else
            if (errno == EAGAIN || errno == EWOULDBLOCK || errno == ECONNREFUSED) break;
    #endif
            break; // Critical error (break to avoid freeze)
        }
        if (r < (ssize_t) sizeof(RUDPHeader_St)) break;

        RUDPHeader_St* h = (RUDPHeader_St*) buffer;

        if (h->action == ACTION_CODE_LOBBY_ROOM_INFO) {
            char ip_str[INET_ADDRSTRLEN];
            // Since we use recv (connected), the 'from' is already known, 
            // but for Room Info (broadcast), we might need recvfrom if not connected.
            // HOWEVER, the logic says Room Info is handled here.
            // If the socket is connected, we might not receive broadcasts.
            // Let's keep it robust.
            inet_ntop(AF_INET, &from.sin_addr, ip_str, sizeof(ip_str));
            addDiscoveredRoom(ip_str, (char*) (buffer + sizeof(RUDPHeader_St)));
            continue;
        }

        if (h->action == ACTION_CODE_JOIN_ACK) {
            u16 temp_id;
            memcpy(&temp_id, buffer + sizeof(RUDPHeader_St), sizeof(u16)); 
            my_id = ntohs(temp_id);
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

        if (h->action == ACTION_CODE_LOBBY_MOVE && rudpProcessIncoming(&serverConnection, h)) {
            if (sid == my_id) continue;
            if (sid >= 0 && sid < MAX_CLIENTS) {
                PlayerNet_St net;
                memcpy(&net, buffer + sizeof(RUDPHeader_St), sizeof(PlayerNet_St));
                
                // If it's the first time we see this player, teleport them
                if (!game->otherPlayers[sid].active) {
                    game->otherPlayers[sid].position = (Vector2){ net.x, net.y };
                    game->otherPlayers[sid].targetPosition = (Vector2){ net.x, net.y };
                    game->otherPlayers[sid].angle = net.angle;
                    game->otherPlayers[sid].textureId = net.textureId;
                    game->otherPlayers[sid].radius = 20.0f; // Default radius
                } else {
                    // Update target for interpolation
                    game->otherPlayers[sid].targetPosition = (Vector2){ net.x, net.y };
                    game->otherPlayers[sid].angle = net.angle;
                    game->otherPlayers[sid].textureId = net.textureId;
                }
                strncpy(game->otherPlayers[sid].name, net.name, 31);
                game->otherPlayers[sid].name[31] = '\0';
                game->otherPlayers[sid].active = true;
            }
            continue;
        }

        if (h->action == ACTION_CODE_QUIT_GAME) {
            if (sid >= 0 && sid < MAX_CLIENTS) game->otherPlayers[sid].active = false;
            continue;
        }

        if ((h->action == ACTION_GAME_DATA || h->action == ACTION_CODE_LOBBY_CHAT) && rudpProcessIncoming(&serverConnection, h)) {
            GameTLVHeader_St* g = (GameTLVHeader_St*) (buffer + sizeof(RUDPHeader_St));
            void* payload = (u8*) g + sizeof(GameTLVHeader_St);
            
            // Global chat processing
            if (g->action == ACTION_CODE_LOBBY_CHAT) {
                if (sid != my_id) {
                    const char* sender_name = (sid == 999) ? "SERVEUR" : TextFormat("Joueur %d", sid);
                    addChatMessage(sender_name, (char*) payload);
                }
            }

            // Dispatch to module (only if it's really game data for a specific game)
            if (g->game_id != 0xFF && game_registry[g->game_id]) {
                game_registry[g->game_id]->on_data(sid, g->action, payload, g->length);
            }
            continue;
        }

        if (h->action == ACTION_CODE_LOBBY_SWITCH_GAME) {
            if (rudpProcessIncoming(&serverConnection, h)) {
                u8 target_id = buffer[sizeof(RUDPHeader_St)];
                printf("[CLIENT] Ordre de switch reçu vers le jeu %d\n", target_id);
                switch_minigame(target_id);
            }
            continue;
        }
    }
}

int main(void) {
    force_kill_server();
    g_progress = LoadProgress();
    
    // Update AP tiers based on current leaderboard placements
    for (int g = 1; g < MAX_GAMES_PROGRESS; g++) {
        Leaderboard_St lb = LoadLeaderboard(g);
        g_progress.total_players[g][0][0] = lb.count;
        g_progress.current_ap[g] = CalculateAPTier(g_progress.leaderboard_rank[g][0][0], lb.count);
    }
    
    InitMenus();

    if (lobby_initGame(&g_lobbyGame) != OK) {
        log_fatal("Couldn't load the lobby properly.");
        return 1;
    }

    applySystemSettings();

    g_lobbyGame->currentState = GAME_STATE_DISCONNECTED;
    initConnectionScreen();
    
    for (int i = 0; i < MAX_CLIENTS; i++) g_lobbyGame->otherPlayers[i].active = false;

    register_minigame(&KingForFourClientModule);
    register_minigame(&ChessClientModule);
    register_minigame(&RubikClientModule);

    float move_timer = 0;

    // ── Main loop ────────────────────────────────────────────────────────────
    while (!WindowShouldClose()) {
        float dt = GetFrameTime();
        if (dt > 0.1f) dt = 0.1f;
        receiveNetworkData(g_lobbyGame);

        UpdateMenu();
        if (g_currentMenu != MENU_NONE) {
            if (g_currentMenu == MENU_PAUSE && IsKeyPressed(KEY_ESCAPE)) g_currentMenu = MENU_NONE;
        }

        if (g_currentMenu == MENU_NONE) {
            updateChat();

            Player_St* local_p = &g_lobbyGame->player;

            if (g_lobbyGame->currentState == GAME_STATE_LOBBY && active_game_id == 0) {
                // Lobby network sync (send pos) - Throttled at 60Hz
                move_timer += dt;
                static Vector2 lastSent = {0};
                if (move_timer >= 0.016f && (local_p->position.x != lastSent.x || local_p->position.y != lastSent.y)) {
                    RUDPHeader_St h; rudpGenerateHeader(&serverConnection, ACTION_CODE_LOBBY_MOVE, &h);
                    h.sender_id = htons((u16)my_id);
                    PlayerNet_St net = {
                        .x = local_p->position.x,
                        .y = local_p->position.y,
                        .angle = local_p->angle,
                        .textureId = (u8)local_p->textureId,
                        .active = true
                    };
                    strncpy(net.name, local_p->name, 31);
                    net.name[31] = '\0';
                    u8 buf[1024];
                    memcpy(buf, &h, sizeof(h));
                    memcpy(buf + sizeof(h), &net, sizeof(PlayerNet_St));
                    if (networkSocket != -1) {
                        send(networkSocket, buf, sizeof(h) + sizeof(PlayerNet_St), 0);
                    }
                    lastSent = local_p->position;
                    move_timer = 0;
                }

                // Game trigger check
                for (u8 i = 1; i < __miniGameCount; i++) {
                    if (CheckCollisionCircleRec(local_p->position, local_p->radius, g_lobbyGame->miniGameManager.gameHitboxes[i])) {
                        const char* game_name = (game_registry[i]) ? game_registry[i]->name : "UNKNOWN";
                        DrawText(TextFormat("APPUYEZ SUR ENTRÉE POUR JOUER : %s", game_name), 
                                 GetScreenWidth()/2 - MeasureText(TextFormat("APPUYEZ SUR ENTRÉE POUR JOUER : %s", game_name), 20)/2, 
                                 GetScreenHeight() - 100, 20, GOLD);
                        
                        if (IsKeyPressed(KEY_ENTER)) {
                            u8 target_id = i;
                            if (networkSocket != -1) {
                                RUDPHeader_St h; rudpGenerateHeader(&serverConnection, ACTION_CODE_LOBBY_SWITCH_GAME, &h);
                                h.sender_id = htons((u16)my_id);
                                u8 buf[64];
                                memcpy(buf, &h, sizeof(h));
                                buf[sizeof(h)] = target_id;
                                send(networkSocket, buf, sizeof(h) + 1, 0);
                                g_lobbyGame->currentState = GAME_STATE_WAITING_SWITCH; 
                                printf("[SYSTEM] Demande de switch envoyée vers jeu %d, attente confirmation...\n", target_id);
                            } else {
                                printf("[SYSTEM] Mode Solo : Lancement direct du jeu %d\n", target_id);
                                switch_minigame(target_id);
                            }
                        }
                        break;
                    }
                }
            }
        }

        BeginDrawing();
        ClearBackground(RAYWHITE);

        if (g_currentMenu != MENU_NONE && g_currentMenu != MENU_PAUSE) {
            DrawMenu();
        } else {
            switch (g_lobbyGame->currentState) {
                case GAME_STATE_DISCONNECTED:
                    if (updateConnectionScreen()) {
                        initNetwork(getEnteredIP());
                        strncpy(g_lobbyGame->player.name, getEnteredPseudo(), 31);
                        g_lobbyGame->player.name[31] = '\0';
                        g_lobbyGame->currentState = GAME_STATE_CONNECTING;
                    }
                    drawConnectionScreen();
                    break;

                case GAME_STATE_CONNECTING:
                    DrawText("CONNEXION EN COURS...", 100, 100, 30, GRAY);
                    break;

                case GAME_STATE_LOBBY:
                case GAME_STATE_WAITING_SWITCH:
                    lobby_gameLoop(g_lobbyGame);
                    if (g_lobbyGame->currentState == GAME_STATE_WAITING_SWITCH) {
                        DrawText("CHARGEMENT DU JEU...", GetScreenWidth()/2 - 100, GetScreenHeight()/2, 20, WHITE);
                    }
                    break;

                case GAME_STATE_INGAME:
                    if (game_registry[active_game_id]) {
                        GameClientInterface_St* module = game_registry[active_game_id];
                        if (module->update) module->update(dt);
                        
                        // Re-check state because update() might have triggered a scene switch
                        if (g_lobbyGame->currentState == GAME_STATE_INGAME && game_registry[active_game_id]) {
                            if (game_registry[active_game_id]->draw) game_registry[active_game_id]->draw();
                        }
                    }
                    break;
                default: break;
            }

            if (g_currentMenu == MENU_PAUSE) DrawMenu();

            drawChat();
        }
        
        DrawFPS(10, 10);
        EndDrawing();
    }

    SaveProgress(&g_progress);
    kill_server();
    lobby_freeGame(&g_lobbyGame);

    return 0;
}

// End of main.c
