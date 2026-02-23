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

#include "core/game.h"              // GameScene_Et, general game types

#include "lobbyAPI.h"
#include "APIs/tetrisAPI.h"
#include "ui/connection_screen.h"

#include "rudp_core.h"
#include "utils/globals.h"

#include <arpa/inet.h>
#include <fcntl.h>

#include "APIs/tetrisAPI.h"

/** @brief Current state of the game. */
static GameState currentState = GAME_STATE_CONNECTION;

/** @brief Global network socket for the client. */
int network_socket = -1;

/** @brief RUDP connection state for the server. */
RUDP_Connection server_conn;

/** @brief Global player structure for the local client. */
Player_st player = { .position = { 400, 300 }, .radius = 20, .active = true };

/** @brief Array of other players in the lobby. */
Player_st otherPlayers[MAX_CLIENTS];

/** @brief Registry of available mini-game modules. */
static MiniGameModule* game_registry[256] = {0};

/** @brief ID of the currently active mini-game module. */
uint8_t active_game_id = 0;


extern MiniGameModule LobbyModule; 
extern MiniGameModule KingForFourClientModule;

/**
 * @brief Switches the active mini-game to the specified ID.
 * @param game_id The ID of the mini-game to switch to.
 */
void switch_minigame(uint8_t game_id) {
    if (game_registry[game_id]) {
        active_game_id = game_id;
    }
}

/**
 * @brief Registers a mini-game module in the registry and initializes it.
 * @param module Pointer to the mini-game module structure.
 */
void register_minigame(MiniGameModule* module) {
    if (module && module->id < 256) {
        game_registry[module->id] = module;
        if (module->init) module->init();
        printf("[SYSTEM] Module '%s' (ID:%d) enregistré.\n", module->name, module->id);
    }
}

/**
 * @brief Ensures that the network socket exists, creating it if necessary.
 */
void ensure_socket_exists() {
    if (network_socket != -1) return;
    network_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (network_socket >= 0) {
        int brd = 1;
        setsockopt(network_socket, SOL_SOCKET, SO_BROADCAST, &brd, sizeof(brd));
        fcntl(network_socket, F_SETFL, O_NONBLOCK);
    }
}

/**
 * @brief Broadcasts a query to discover available lobby servers.
 */
void discover_servers(void) {
    ensure_socket_exists();
    struct sockaddr_in addr = { .sin_family = AF_INET, .sin_port = htons(SERVER_PORT), .sin_addr.s_addr = INADDR_BROADCAST };
    RUDP_Header q = { .action = LOBBY_ROOM_QUERY };
    sendto(network_socket, &q, sizeof(RUDP_Header), 0, (struct sockaddr*)&addr, sizeof(addr));
}

/**
 * @brief Initializes the network connection to a target IP.
 * @param target_ip The IP address of the server to connect to.
 */
void init_network(const char* target_ip) {
    ensure_socket_exists();
    struct sockaddr_in addr = { .sin_family = AF_INET, .sin_port = htons(SERVER_PORT) };
    inet_pton(AF_INET, target_ip, &addr.sin_addr);
    connect(network_socket, (struct sockaddr *)&addr, sizeof(addr));
    RUDP_InitConnection(&server_conn);
    
    GameTLVHeader tlv = { .game_id = 0, .action = LOBBY_JOIN, .length = 0 };
    uint8_t buffer[sizeof(RUDP_Header) + sizeof(GameTLVHeader)];
    RUDP_Header h; RUDP_GenerateHeader(&server_conn, ACTION_GAME_DATA, &h);
    memcpy(buffer, &h, sizeof(h)); memcpy(buffer+sizeof(h), &tlv, sizeof(tlv));
    send(network_socket, buffer, sizeof(buffer), 0);
}

/**
 * @brief Receives and processes incoming network data packets.
 */
void receive_network_data(void) {
    if (network_socket == -1) return;
    uint8_t buffer[2048];
    struct sockaddr_in from; socklen_t len = sizeof(from);
    
    while (1) {
        ssize_t r = recvfrom(network_socket, buffer, sizeof(buffer), 0, (struct sockaddr*)&from, &len);
        if (r < (ssize_t)sizeof(RUDP_Header)) break;

        RUDP_Header* h = (RUDP_Header*)buffer;

        if (h->action == LOBBY_ROOM_INFO) {
            AddDiscoveredRoom(inet_ntoa(from.sin_addr), (char*)(buffer + sizeof(RUDP_Header)));
            continue;
        }

        if (h->action == 0x20 /* LOBBY_SWITCH_GAME */ && RUDP_ProcessIncoming(&server_conn, h)) {
            uint8_t target_game_id = *(uint8_t*)(buffer + sizeof(RUDP_Header));
            printf("[SYSTEM] Switch réseau vers module ID:%d\n", target_game_id);
            switch_minigame(target_game_id);
            continue;
        }

        if (h->action == ACTION_GAME_DATA && RUDP_ProcessIncoming(&server_conn, h)) {
            GameTLVHeader* g = (GameTLVHeader*)(buffer + sizeof(RUDP_Header));
            void* payload = (uint8_t*)g + sizeof(GameTLVHeader);
            
            if (game_registry[g->game_id]) {
                game_registry[g->game_id]->on_data(ntohs(h->sender_id), g->action, payload, g->length);
            }
        }
    }
}

/**
 * @brief Updates and renders the lobby scene (one frame).
 * @param dt  Frame time delta (from GetFrameTime())
 *
 * Handles player movement, camera, UI menus, and game zone collision detection.
 */
static void lobby_gameLoop(float dt) {
    static float lobbyTextXPos;

    updatePlayer(&player, platforms, platformCount, dt);
    cam.target = player.position;

    toggleSkinMenu();

    if (isTextureMenuOpen) {
        choosePlayerTexture(&player);
    }

    // Collision check with game zone (tetris example)
    if (CheckCollisionCircleRec(player.position, player.radius, tetrisHitbox)) {
        if (!gameHitGracePeriodActive) {
            currentScene = GAME_SCENE_GAME_NAME;
            needGameInit = true;
            gameHitGracePeriodActive = true;
        }
    } else if (gameHitGracePeriodActive) {
        gameHitGracePeriodActive = false;
    }

    BeginDrawing(); {
        ClearBackground(RAYWHITE);

        BeginMode2D(cam); {
            DrawCircle(0, 0, 10, RED);          // Debug origin marker
            drawPlayer(&player);
            drawPlatforms(platforms, platformCount);
            DrawRectangleRec(tetrisHitbox, RED); // Debug hitbox
        } EndMode2D();

        lobbyTextXPos = (WINDOW_WIDTH - MeasureText("Multi-Mini-Games", 20)) / 2.0f;
        DrawText("Multi-Mini-Games", lobbyTextXPos, 20, 20, PURPLE);

        drawSkinButton();

        if (isTextureMenuOpen) {
            drawMenuTextures();
        }
    } EndDrawing();
}

/**
    @brief Program entry point.
    @return 0 on clean exit, non-zero on early failure
*/
int main(void) {

    // ── Initialization ───────────────────────────────────────────────────────
    Error_Et error = OK;

    LobbyGame_St* game = NULL;
    if (lobby_initGame(&game) == ERROR_ALLOC) {
        log_fatal("Couldn't load the lobby properly.");
        return 1;
    }

    InitConnectionScreen();
    
    for (int i = 0; i < MAX_CLIENTS; i++) {
        otherPlayers[i].active = false;
    }

    register_minigame(&LobbyModule);
    register_minigame(&KingForFourClientModule);

    InitConnectionScreen();
    
    for (int i = 0; i < MAX_CLIENTS; i++) {
        otherPlayers[i].active = false;
    }

    register_minigame(&LobbyModule);
    register_minigame(&KingForFourClientModule);

    // ── Main loop ────────────────────────────────────────────────────────────
    while (!WindowShouldClose()) {
        float dt = GetFrameTime();
        if (dt > 0.1f) dt = 0.1f;
        receive_network_data();

        static bool switch_sent = false;
        if (currentState == GAME_STATE_LOBBY && active_game_id == 0) {
            bool trigger = (checkGameTrigger(&player) == 1) || IsKeyPressed(KEY_K);
            if (trigger && !switch_sent) {
                RUDP_Header leave_h; RUDP_GenerateHeader(&server_conn, 6 /* LOBBY_LEAVE */, &leave_h);
                send(network_socket, &leave_h, sizeof(leave_h), 0);

                uint8_t target_id = 1;
                RUDP_Header h; RUDP_GenerateHeader(&server_conn, 0x20 /* LOBBY_SWITCH_GAME */, &h);
                
                uint8_t buffer[sizeof(RUDP_Header) + 1];
                memcpy(buffer, &h, sizeof(h));
                buffer[sizeof(h)] = target_id;
                
                send(network_socket, buffer, sizeof(buffer), 0);
                switch_sent = true;
                printf("[SYSTEM] Requête de switch vers ID %d envoyée.\n", target_id);
            }
            if (!trigger) switch_sent = false;
        }

        switch (currentState) {
            case STATE_CONNECTION: {
                static float timer = 0; timer += dt;
                if (timer > 2.0f) { discover_servers(); timer = 0; }
                if (UpdateConnectionScreen()) {
                    init_network(GetEnteredIP());
                    currentState = GAME_STATE_LOBBY;
                }

                BeginDrawing(); {
                    ClearBackground(RAYWHITE);
                    DrawConnectionScreen(); 
                } EndDrawing();
            } break;

            case GAME_STATE_LOBBY: {
                if (game_registry[active_game_id]) {
                    game_registry[active_game_id]->update(dt);

                    BeginDrawing(); {
                        ClearBackground(RAYWHITE);
                        game_registry[active_game_id]->draw();
                    } EndDrawing();
                }
            } break;
        }
        /**
            switch (game->subGameManager.currentScene) {
                case GAME_SCENE_LOBBY: {
                    lobby_gameLoop(game);
                } break;

                case GAME_SCENE_TETRIS: {
                    Game_St** miniRef = &game->subGameManager.miniGames[GAME_SCENE_TETRIS];
                    TetrisGame_St** tetrisRef = (TetrisGame_St**) miniRef;
                    if (game->subGameManager.needGameInit) {
                        error = tetris_initGame(tetrisRef);
                        game->subGameManager.needGameInit = false;

                        if (error != OK) {
                            log_fatal("Tetris initialization failed: error %d", error);
                            tetris_freeGame(tetrisRef);
                            game->subGameManager.currentScene = GAME_SCENE_LOBBY;
                            break;
                        }
                    }

                    tetris_gameLoop(*tetrisRef);

                    if (!(*miniRef)->running) {
                        tetris_freeGame(tetrisRef);
                        game->subGameManager.currentScene = GAME_SCENE_LOBBY;
                    }
                } break;

                default:
                    log_error("Invalid GameScene_Et value: %d", game->subGameManager.currentScene);
                    break;
            }
        }
        */
    }

    // ── Cleanup ──────────────────────────────────────────────────────────────
    lobby_freeGame(&game);

    return 0;
}

#define LOGGER_IMPLEMENTATION
#include "logger.h"

#define SYSTEM_SETTINGS_IMPLEMENTATION
#include "systemSettings.h"