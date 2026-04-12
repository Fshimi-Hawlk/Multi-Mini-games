/**
    @file main.c
    @author LeandreB8
    @date 2026-01-12
    @date 2026-03-22
    @brief Program entry point – lobby main loop and game scene manager.

    Contributors:
        - LeandreB8:
            - Implemented basic lobby's logic (initialization, game loop, ...)
        - Fshimi-Hawlk:
            - Moved & reworked lobby's init/loop/free in dedicated lobbyAPI files
            - Implemented sub-game playability via API

    Architecture
    ────────────
    Instead of a growing switch/case, games are registered in the `scenes[]` table.
    Adding a new game requires only:
      1. A new MINI_GAME_ID_XXX entry in MiniGameId_Et (userTypes.h)
      2. A new row in scenes[] below
    The main loop is generic and never changes.
*/

// #include "APIs/lobbyAPI.h"
// #include "APIs/tetrisAPI.h"
// #include "APIs/solitaireAPI.h"
// #include "APIs/suikaAPI.h"
// #include "APIs/bowlingAPI.h"
// #include "APIs/golfAPI.h"
// #include "APIs/snakeAPI.h"
// #include "APIs/polyBlastAPI.h"

// #include "APIs/generalAPI.h"

// // ─────────────────────────────────────────────────────────────────
// // Generic game dispatch table
// // ─────────────────────────────────────────────────────────────────

// /**
//     @brief Function pointer types matching the generalAPI lifecycle contract.
// */
// typedef Error_Et (*InitFn)(BaseGame_St**);
// typedef Error_Et (*LoopFn)(BaseGame_St*);
// typedef Error_Et (*FreeFn)(BaseGame_St**);

// /**
//     @brief One entry per mini-game scene.

//     width / height = 0 means "keep current window size".
// */
// typedef struct {
//     const char* name;
//     InitFn      init;
//     LoopFn      loop;
//     FreeFn      free;
//     int         width;
//     int         height;
// } SceneDesc_St;

// // ── Adapter shims ────────────────────────────────────────────────
// // Each game has its own typed pointer; we cast through BaseGame_St** here.
// // This is safe because BaseGame_St is the first member of every game struct.

// static Error_Et tetris_init_shim(BaseGame_St** p)   { return tetris_initGame((TetrisGame_St**)p);       }
// static Error_Et tetris_loop_shim(BaseGame_St*  p)   { return tetris_gameLoop((TetrisGame_St*)p);        }
// static Error_Et tetris_free_shim(BaseGame_St** p)   { return tetris_freeGame((TetrisGame_St**)p);       }

// static Error_Et solitaire_init_shim(BaseGame_St** p){ return solitaire_initGame((SolitaireGame_St**)p); }
// static Error_Et solitaire_loop_shim(BaseGame_St*  p){ solitaire_gameLoop((SolitaireGame_St*)p); return OK; }
// static Error_Et solitaire_free_shim(BaseGame_St** p){ return solitaire_freeGame((SolitaireGame_St**)p); }

// static Error_Et suika_init_shim(BaseGame_St** p)    { return suika_initGame((SuikaGame_St**)p);         }
// static Error_Et suika_loop_shim(BaseGame_St*  p)    { return suika_gameLoop((SuikaGame_St*)p);          }
// static Error_Et suika_free_shim(BaseGame_St** p)    { return suika_freeGame((SuikaGame_St**)p);         }

// static Error_Et bowling_init_shim(BaseGame_St** p)  { return bowling_initGame((BowlingGame_St**)p);     }
// static Error_Et bowling_loop_shim(BaseGame_St*  p)  { return bowling_gameLoop((BowlingGame_St*)p);      }
// static Error_Et bowling_free_shim(BaseGame_St** p)  { return bowling_freeGame((BowlingGame_St**)p);     }

// static Error_Et golf_init_shim(BaseGame_St** p)     { return golf_initGame((GolfGame_St**)p);           }
// static Error_Et golf_loop_shim(BaseGame_St*  p)     { return golf_gameLoop((GolfGame_St*)p);            }
// static Error_Et golf_free_shim(BaseGame_St** p)     { return golf_freeGame((GolfGame_St**)p);           }

// static Error_Et snake_init_shim(BaseGame_St** p)    { return snake_initGame((SnakeGame_St**)p);         }
// static Error_Et snake_loop_shim(BaseGame_St*  p)    { return snake_gameLoop((SnakeGame_St*)p);          }
// static Error_Et snake_free_shim(BaseGame_St** p)    { return snake_freeGame((SnakeGame_St**)p);         }

// static Error_Et polyBlast_init_shim(BaseGame_St** p)    { return polyBlast_initGame((PolyBlastGame_St**)p);         }
// static Error_Et polyBlast_loop_shim(BaseGame_St*  p)    { return polyBlast_gameLoop((PolyBlastGame_St*)p);          }
// static Error_Et polyBlast_free_shim(BaseGame_St** p)    { return polyBlast_freeGame((PolyBlastGame_St**)p);         }

// /**
//     @brief Scene dispatch table.

//     Index 0 = MINI_GAME_ID_LOBBY (handled separately — lobby is not a mini-game).
//     Index i = MINI_GAME_ID_XXX.
// */
// static const SceneDesc_St scenes[__miniGameIdCount] = {
//     [MINI_GAME_ID_LOBBY]        = { "Lobby",        NULL,                   NULL,                   NULL,                   800,  600 },
//     [MINI_GAME_ID_TETRIS]       = { "Tetris",       tetris_init_shim,       tetris_loop_shim,       tetris_free_shim,       600,  800 },
//     [MINI_GAME_ID_SOLITAIRE]    = { "Solitaire",    solitaire_init_shim,    solitaire_loop_shim,    solitaire_free_shim,    0,    0   },
//     [MINI_GAME_ID_SUIKA]        = { "Suika",        suika_init_shim,        suika_loop_shim,        suika_free_shim,        800,  900 },
//     [MINI_GAME_ID_BOWLING]      = { "Bowling",      bowling_init_shim,      bowling_loop_shim,      bowling_free_shim,      1200, 800 },
//     [MINI_GAME_ID_GOLF]         = { "Golf 3D",      golf_init_shim,         golf_loop_shim,         golf_free_shim,         1280, 720 },
//     [MINI_GAME_ID_SNAKE]        = { "Snake",        snake_init_shim,        snake_loop_shim,        snake_free_shim,        800,  600 },
//     [MINI_GAME_ID_POLY_BLAST]   = { "Poly Blast",   polyBlast_init_shim,    polyBlast_loop_shim,    polyBlast_free_shim,    800,  600 },
// };

// // ─────────────────────────────────────────────────────────────────
// // Helpers
// // ─────────────────────────────────────────────────────────────────

// static void returnToLobby(LobbyGame_St* game) {
//     game->subGameManager.currentScene = MINI_GAME_ID_LOBBY;
//     game->player.position = (Vector2){ PLAYER_SPAWN_X, PLAYER_SPAWN_Y };  // Teleport to center on lobby re-entry
//     game->player.velocity = (Vector2){ 0, 0 };   // Reset velocity to prevent momentum carryover
//     game->player.coyoteTimer = COYOTE_TIME;       // Reset jump states
//     game->player.jumpBuffer = 0.0f;
//     // Restore lobby window size (1200x800 is standard for lobby)
//     systemSettings.video.width = 1200;
//     systemSettings.video.height = 800;
//     systemSettings.video.fps = 60;
//     systemSettings.video.title = "Lobby";
//     applySystemSettings();
// }

// static void applySceneWindowSize(const SceneDesc_St* desc) {
//     if (desc->width > 0 && desc->height > 0) {
//         systemSettings.video.width  = desc->width;
//         systemSettings.video.height = desc->height;
//         applySystemSettings();
//     }
// }

// // ─────────────────────────────────────────────────────────────────
// // Entry point
// // ─────────────────────────────────────────────────────────────────

// /**
//     @brief Program entry point.
//     @return 0 on clean exit, non-zero on early failure
// */
// s32 main(void) {
//     LobbyGame_St* game = NULL;
//     if (lobby_initGame(&game) == ERROR_ALLOC) {
//         log_fatal("Couldn't load the lobby properly.");
//         return 1;
//     }

//     // Disable ESC to close window by default - games handle ESC to return to lobby
//     SetExitKey(KEY_NULL);

//     bool skipWindowClose = false;
//     while (!WindowShouldClose() || skipWindowClose) {
//         // Reset skip flag at start of each iteration
//         if (skipWindowClose) skipWindowClose = false;

//         MiniGameId_Et scene = game->subGameManager.currentScene;

//         // ── Lobby ────────────────────────────────────────────────
//         if (scene == MINI_GAME_ID_LOBBY) {
//             // In lobby, ESC closes the game
//             if (IsKeyPressed(KEY_ESCAPE) && WindowShouldClose()) {
//                 break;
//             }
//             lobby_gameLoop(game);
//             continue;
//         }

//         // Re-disable ESC for games (in case we returned from lobby)
//         SetExitKey(KEY_NULL);

//         // ── Mini-game generic dispatch ───────────────────────────
//         if (scene >= __miniGameIdCount || scenes[scene].init == NULL) {
//             log_error("Invalid or unregistered MiniGameId_Et value: %d", scene);
//             returnToLobby(game);
//             continue;
//         }

//         const SceneDesc_St* desc    = &scenes[scene];
//         BaseGame_St**       miniRef = &game->subGameManager.miniGames[scene];

//         // Init (once)
//         if (game->subGameManager.needGameInit) {
//             Error_Et err = desc->init(miniRef);
//             game->subGameManager.needGameInit = false;

//             if (err != OK) {
//                 log_fatal("%s initialization failed (error %d)", desc->name, err);
//                 if (*miniRef) desc->free(miniRef);
//                 returnToLobby(game);
//                 continue;
//             }
//             applySceneWindowSize(desc);
//         }

//         // Loop frame
//         if (*miniRef) desc->loop(*miniRef);

//         // Detect end
//         if (!*miniRef || !(*miniRef)->running) {
//             if (*miniRef) desc->free(miniRef);
//             game->subGameManager.needGameInit = true;  // Force reinit for next game
//             returnToLobby(game);
//             continue;
//         }
//     }

//     lobby_freeGame(&game);
//     return 0;
// }

/*
    @author Fshimi Hawlk
    @author LeandreB8
    @author i-Charlys
    @date 2026-02-08
    @date 2026-04-02
    @brief Program entry point for the lobby client – lobby main loop, game scene manager, networking and module dispatching.
*/

#include "ui/menus.h"
#include "ui/connectionScreen.h"
#include "ui/roomSelector.h"
#include "ui/waitingRoom.h"

#include "setups/app.h"

#include "utils/globals.h"

#include "leaderboard.h"


s32 networkSocket = -1;
RUDPConnection_St serverConnection; 

static MiniGameId_Et currentMiniGameID = MINI_GAME_ID_LOBBY;
static GameClientInterface_St* currentMiniGame = NULL;

extern GameClientInterface_St lobbyClientInterface;
extern GameClientInterface_St kingForFourClientInterface;
extern GameClientInterface_St bingoClientInterface;
extern GameClientInterface_St ChessClientModule;
extern GameClientInterface_St RubikClientModule;
extern GameClientInterface_St editorClientInterface;

// Pointers to the mini-game client interfaces
static GameClientInterface_St* miniGameInterfaces[__miniGameIdCount] = {
    [MINI_GAME_ID_BINGO]          = &bingoClientInterface,
    [MINI_GAME_ID_BOWLING]        = NULL,
    [MINI_GAME_ID_CHESS]          = &ChessClientModule,
    [MINI_GAME_ID_DISC_REVERSAL]  = NULL,
    [MINI_GAME_ID_DROP_FOUR]      = NULL,
    [MINI_GAME_ID_EDITOR]         = &editorClientInterface,
    [MINI_GAME_ID_KING_FOR_FOUR]  = &kingForFourClientInterface,
    [MINI_GAME_ID_LOBBY]          = &lobbyClientInterface,
    [MINI_GAME_ID_MINI_GOLF]      = NULL,
    [MINI_GAME_ID_POLY_BLAST]     = NULL,
    [MINI_GAME_ID_SNAKE]          = NULL,
    [MINI_GAME_ID_SOLO_CARDS]     = NULL,
    [MINI_GAME_ID_SUIKA]          = NULL,
    [MINI_GAME_ID_TETROMINO_FALL] = NULL,
    [MINI_GAME_ID_TWIST_CUBE]     = &RubikClientModule,
};

static bool server_spawned = false;

#ifndef _WIN32
static pid_t server_pid = -1;
#endif

void kill_server(void) {
    if (server_spawned) {
        log_info("Arrêt du serveur local...");
#ifdef _WIN32
        system("taskkill /F /IM server.exe > nul 2>&1");
#else
        if (server_pid > 0) {
            kill(server_pid, SIGTERM);
            server_pid = -1;
        } else {
            system("killall server 2>/dev/null");
        }
#endif
        server_spawned = false;
    }
}

void discoverServers(void) {
    if (networkSocket < 0) {
        networkSocket = socket(AF_INET, SOCK_DGRAM, 0);
        if (networkSocket < 0) return;
        int broadcastEnable = 1;
        setsockopt(networkSocket, SOL_SOCKET, SO_BROADCAST, &broadcastEnable, sizeof(broadcastEnable));
        int flags = fcntl(networkSocket, F_GETFL, 0);
        fcntl(networkSocket, F_SETFL, flags | O_NONBLOCK);
        log_info("[NET] Discovery socket initialized");
    }

    struct sockaddr_in broadcastAddr;
    memset(&broadcastAddr, 0, sizeof(broadcastAddr));
    broadcastAddr.sin_family = AF_INET;
    broadcastAddr.sin_port = htons(8080);
    broadcastAddr.sin_addr.s_addr = INADDR_BROADCAST;

    // Localhost specifically for local tests
    struct sockaddr_in localAddr;
    memset(&localAddr, 0, sizeof(localAddr));
    localAddr.sin_family = AF_INET;
    localAddr.sin_port = htons(8080);
    localAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    RUDPHeader_St h_disc = { .action = ACTION_CODE_DISCOVERY_QUERY, .sequence = 0, .ack = 0, .ack_bitfield = 0, .sender_id = 0 };
    sendto(networkSocket, &h_disc, sizeof(h_disc), 0, (struct sockaddr*)&broadcastAddr, sizeof(broadcastAddr));
    sendto(networkSocket, &h_disc, sizeof(h_disc), 0, (struct sockaddr*)&localAddr, sizeof(localAddr));

    log_debug("Discovery queries sent...");
}

void initNetwork(const char* ip, const char* pseudo) {
    if (networkSocket != -1) close(networkSocket);

    networkSocket = socket(AF_INET, SOCK_DGRAM, 0);
    if (networkSocket < 0) {
        log_error("Failed to create socket");
        return;
    }

    struct sockaddr_in serverAddr;
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(8080);
    inet_pton(AF_INET, ip, &serverAddr.sin_addr);

    rudpInitConnection(&serverConnection);

    // Connect socket to allow using send() instead of sendto() for gameplay
    if (connect(networkSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        log_error("Failed to connect socket");
    }

    // Send Join Game with Pseudo
    RUDPHeader_St h;
    rudpGenerateHeader(&serverConnection, ACTION_CODE_JOIN_GAME, &h);
    u8 buf[128];
    memcpy(buf, &h, sizeof(h));
    strncpy((char*)buf + sizeof(h), pseudo, 31);

    send(networkSocket, buf, sizeof(h) + strlen(pseudo) + 1, 0);
    log_info("Sent JOIN_GAME for player '%s'", pseudo);

    int flags = fcntl(networkSocket, F_GETFL, 0);
    fcntl(networkSocket, F_SETFL, flags | O_NONBLOCK);

    log_info("Network initialized towards %s:8080", ip);
}

void receiveNetworkData(void) {
    if (networkSocket < 0) return;

    u8 buffer[2048];
    struct sockaddr_in fromAddr;
    socklen_t fromLen = sizeof(fromAddr);

    while (1) {
        ssize_t bytesRead = recvfrom(networkSocket, buffer, sizeof(buffer), 0, (struct sockaddr*)&fromAddr, &fromLen);
        
        if (bytesRead < 0) {
            if (errno != EAGAIN && errno != EWOULDBLOCK) {
                log_error("[NET] recvfrom error: %s", strerror(errno));
            }
            break;
        }

        if (bytesRead >= (ssize_t)sizeof(RUDPHeader_St)) {
            RUDPHeader_St header;
            memcpy(&header, buffer, sizeof(RUDPHeader_St));
            
            // Handle discovery outside of RUDP state machine
            if (header.action == ACTION_CODE_DISCOVERY_INFO && ntohs(header.sender_id) == 999) {
                char* info = (char*)buffer + sizeof(RUDPHeader_St);
                log_info("[DISCOVERY] Found Server at %s: %s", inet_ntoa(fromAddr.sin_addr), info);
                extern void addDiscoveredRoom(const char* ip, const char* name);
                addDiscoveredRoom(inet_ntoa(fromAddr.sin_addr), info);
                continue;
            }

            if (rudpProcessIncoming(&serverConnection, &header)) {
                u8* payload = buffer + sizeof(RUDPHeader_St);
                u16 payloadLen = (u16) (bytesRead - sizeof(RUDPHeader_St));
                u16 senderId = ntohs(header.sender_id);

                if (header.action == ACTION_CODE_JOIN_ERROR) {
                    log_error("[NET] Join rejected by server: %s", (char*)payload);
                    setConnectionError((char*)payload);
                    lobby_game.currentState = GAME_STATE_CONNECTION;
                    // Reset network state if needed
                    rudpInitConnection(&serverConnection);
                }
                else if (header.action == ACTION_CODE_LOBBY_ROOM_INFO) {
                    extern void handleRoomList(const void* data, int count);
                    handleRoomList(payload, payloadLen / sizeof(RoomInfo_St));
                }
                else if (header.action == ACTION_CODE_LOBBY_SWITCH_GAME) {
                    if (payloadLen >= 2) {
                        u8 nextGame = payload[0];
                        u8 roomId = payload[1];
                        log_info("[NET] Server confirmed switch to Game %d, Room %d", nextGame, roomId);
                        
                        if (nextGame < __miniGameIdCount) {
                            if (currentMiniGame && currentMiniGame->destroy) {
                                currentMiniGame->destroy();
                            }
                            currentMiniGameID = (MiniGameId_Et) nextGame;
                            currentMiniGame = miniGameInterfaces[currentMiniGameID];
                            if (currentMiniGame && currentMiniGame->init) currentMiniGame->init();
                            
                            closeRoomSelector();
                            lobby_game.currentState = (nextGame == MINI_GAME_ID_LOBBY) ? GAME_STATE_GAMEPLAY : GAME_STATE_INGAME;
                            if (nextGame == MINI_GAME_ID_LOBBY) initWaitingRoom();
                        }
                    }
                }
                else if (header.action == ACTION_CODE_GAME_DATA) {
                    if (payloadLen >= sizeof(GameTLVHeader_St)) {
                        GameTLVHeader_St tlv;
                        memcpy(&tlv, payload, sizeof(tlv));
                        u16 tlv_data_len = ntohs(tlv.length);
                        
                        if (tlv_data_len <= payloadLen - sizeof(GameTLVHeader_St)) {
                            if (tlv.game_id < __miniGameIdCount && miniGameInterfaces[tlv.game_id]) {
                                miniGameInterfaces[tlv.game_id]->on_data(senderId, tlv.action, payload + sizeof(tlv), tlv_data_len);
                            }
                        }
                    }
                } 
                else if (currentMiniGame && currentMiniGame->on_data) {
                    if (header.action < firstAvailableActionCode || header.action == ACTION_CODE_JOIN_ACK) {
                        currentMiniGame->on_data(senderId, header.action, payload, payloadLen);
                    }
                }
            }
        }
    }
}

void spawn_server(void) {
    if (!server_spawned) {
        log_info("Lancement du serveur local...");
#ifdef _WIN32
        system("start /B server.exe");
        server_spawned = true;
#else
        server_pid = fork();
        if (server_pid == 0) {
            // Child process
            execl("./build/bin/server", "server", (char*)NULL);
            perror("execl failed");
            exit(1);
        } else if (server_pid > 0) {
            server_spawned = true;
        } else {
            log_error("Fork failed to spawn server");
        }
#endif
    }
}

// Backup of lobby terrains before entering editor (so editor changes don't persist in lobby)
static TerrainVec_St lobbyTerrainBackup = {0};

void switch_minigame(u8 game_id) {
    if (game_id < __miniGameIdCount && miniGameInterfaces[game_id]) {
        // Notify server that we are leaving the current minigame room (if any)
        if (currentMiniGameID != MINI_GAME_ID_LOBBY && currentMiniGameID != MINI_GAME_ID_EDITOR && game_id == MINI_GAME_ID_LOBBY) {
            GameTLVHeader_St tlv = { .game_id = currentMiniGameID, .action = ACTION_CODE_QUIT_GAME, .length = 0 };
            RUDPHeader_St h;
            rudpGenerateHeader(&serverConnection, ACTION_CODE_GAME_DATA, &h);
            h.sender_id = htons((u16) lobby_game.clientId);
            u8 buf[64];
            memcpy(buf, &h, sizeof(h));
            memcpy(buf + sizeof(h), &tlv, sizeof(tlv));
            send(networkSocket, buf, sizeof(h) + sizeof(tlv), 0);
        }

        // Leaving editor → restore lobby terrains
        if (currentMiniGameID == MINI_GAME_ID_EDITOR && game_id != MINI_GAME_ID_EDITOR) {
            da_clear(&terrains);
            if (lobbyTerrainBackup.count > 0)
                da_append_many(&terrains, lobbyTerrainBackup.items, lobbyTerrainBackup.count);
        }
        // Entering editor → backup lobby terrains
        if (game_id == MINI_GAME_ID_EDITOR) {
            da_clear(&lobbyTerrainBackup);
            if (terrains.count > 0)
                da_append_many(&lobbyTerrainBackup, terrains.items, terrains.count);
        }

        if (currentMiniGame && currentMiniGame->destroy) {
            currentMiniGame->destroy();
        }
        currentMiniGameID = (MiniGameId_Et) game_id;
        currentMiniGame = miniGameInterfaces[currentMiniGameID];
        if (currentMiniGame && currentMiniGame->init) currentMiniGame->init();

        // Ensure UI state is cleared when switching
        lobby_game.currentState = GAME_STATE_GAMEPLAY;
        closeRoomSelector();

        if (game_id == MINI_GAME_ID_LOBBY) {
            lobby_game.currentState = GAME_STATE_GAMEPLAY;
            lobby_game.editorMode = false;
            initWaitingRoom(); // Réinitialise l'overlay pour éviter qu'il reste visible au retour lobby
        } else {
            lobby_game.currentState = GAME_STATE_INGAME;
            if (game_id == MINI_GAME_ID_EDITOR) lobby_game.editorMode = true;
        }
        log_info("Switched to mini-game ID: %d", game_id);
    }
}

int main(void) {
    g_progress = LoadProgress();
    
    if (lobby_initApp() != OK) return 1;

    InitMenus();

    miniGameInterfaces[MINI_GAME_ID_LOBBY]->init();
    currentMiniGame = miniGameInterfaces[MINI_GAME_ID_LOBBY];
    
    initConnectionScreen();
    // Use the default pseudo from the connection screen as the initial player name
    strncpy(lobby_game.player.name, getEnteredPseudo(), 31);

    initRoomSelector();
    initWaitingRoom();

    while (!WindowShouldClose()) {
        float dt = GetFrameTime();
        if (dt > 0.1f) dt = 0.1f;
        
        receiveNetworkData();

        if (IsWindowResized()) {
            systemSettings.video.width  = GetScreenWidth();
            systemSettings.video.height = GetScreenHeight();
        }

        UpdateMenu();
        bool selectorActive = updateRoomSelector();
        // L'overlay "salle d'attente" du lobby n'est actif qu'en lobby :
        // chaque module de jeu gère sa propre phase pré-partie.
        if (currentMiniGameID == MINI_GAME_ID_LOBBY) updateWaitingRoom();

        BeginDrawing();
        ClearBackground(SKYBLUE);

        if (g_currentMenu != MENU_NONE) {
            DrawMenu();
        } 
        else if (lobby_game.currentState == GAME_STATE_CONNECTION) {
            static f32 disc_timer = 0;
            disc_timer += dt;
            if (disc_timer > 2.0f) { discoverServers(); disc_timer = 0; }
            
            if (updateConnectionScreen()) {
                const char* pseudo = getEnteredPseudo();
                initNetwork(getEnteredIP(), pseudo);
                strncpy(lobby_game.player.name, pseudo, 31);
                lobby_game.currentState = GAME_STATE_ROOM_LIST;
                openRoomSelector(-1); // Open global room list
            }
            drawConnectionScreen();
        } 
        else if (lobby_game.currentState == GAME_STATE_ROOM_LIST) {
            drawRoomSelector();
        }
        else {
            // Gameplay loop
            static MiniGameId_Et lastTriggerID = MINI_GAME_ID_LOBBY;

            if (currentMiniGameID == MINI_GAME_ID_LOBBY && !lobby_game.editorMode && !selectorActive) {
                MiniGameId_Et triggerID = MINI_GAME_ID_LOBBY;
                for (int i = 1; i < __miniGameIdCount; i++) {
                    if (gameZones[i].hitbox.width > 0 &&
                        CheckCollisionCircleRec(lobby_game.player.position, lobby_game.player.radius, gameZones[i].hitbox)) {
                        triggerID = i;
                        break;
                    }
                }

                if (triggerID != MINI_GAME_ID_LOBBY) {
                    if (lastTriggerID != triggerID) {
                        log_info("[ZONE] Entered Zone for Game ID %d", triggerID);
                        lastTriggerID = triggerID;
                    }

                    const char* gname = gameZones[triggerID].name;
                    DrawText(TextFormat("APPUYEZ SUR ENTRÉE POUR : %s", gname), 
                             GetScreenWidth()/2 - MeasureText(TextFormat("APPUYEZ SUR ENTRÉE POUR : %s", gname), 20)/2, 
                             GetScreenHeight() - 100, 20, GREEN);

                    if (IsKeyPressed(KEY_ENTER)) {
                        // Solo mode (no server) or editor: switch directly without room selector
                        if (triggerID == MINI_GAME_ID_EDITOR || networkSocket < 0) {
                            switch_minigame(triggerID);
                        } else {
                            openRoomSelector(triggerID);
                        }
                    }
                } else {
                    if (lastTriggerID != MINI_GAME_ID_LOBBY) {
                        closeRoomSelector();
                        lastTriggerID = MINI_GAME_ID_LOBBY;
                    }
                }
            }

            if (currentMiniGame) {
                currentMiniGame->update(dt);
                currentMiniGame->draw();
            }
            
            drawRoomSelector();
            if (currentMiniGameID == MINI_GAME_ID_LOBBY) drawWaitingRoom();
        }

        DrawFPS(10, 10);
        EndDrawing();
    }

    SaveProgress(&g_progress);

    if (networkSocket != -1) {
        RUDPHeader_St h;
        rudpGenerateHeader(&serverConnection, ACTION_CODE_QUIT_GAME, &h);
        h.sender_id = htons((u16)lobby_game.clientId);
        send(networkSocket, &h, sizeof(h), 0);
    }

    kill_server();
    lobby_freeApp();

    return 0;
}