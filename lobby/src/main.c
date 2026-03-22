/**
    @file main.c
    @author Fshimi Hawlk
    @author LeandreB8
    @author i-Charlys (CAILLON Charles)
    @date 2026-02-08
    @date 2026-03-20
    @brief Program entry point for the lobby client – lobby main loop, game scene manager, networking and module dispatching.
*/

#include "core/game.h"
#include "lobbyAPI.h"
#include "ui/connection_screen.h"
#include "ui/menus.h"
#include "utils/chat.h"
#include "rudp_core.h"
#include "utils/globals.h"
#include "firstparty/progress.h"
#include <arpa/inet.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>

/** @brief Global player progress. */
PlayerProgress_St g_progress;

/** @brief Internal state of the client. */
typedef enum {
    CLIENT_STATE_DISCONNECTED,
    CLIENT_STATE_CONNECTING,
    CLIENT_STATE_LOBBY,
    CLIENT_STATE_WAITING_SWITCH,
    CLIENT_STATE_INGAME
} ClientState_Et;

static ClientState_Et clientState = CLIENT_STATE_DISCONNECTED;

/** @brief Global network socket for the client. */
s32 network_socket = -1;

/** @brief RUDP connection state for the server. */
RUDP_Connection server_conn;

/** @brief My internal client ID assigned by the server. */
s32 my_id = -1;

/** @brief Registry of available mini-game modules. */
static MiniGameModule* game_registry[256] = {0};

/** @brief ID of the currently active mini-game module. */
u8 active_game_id = 0;

extern MiniGameModule KingForFourClientModule;

/**
 * @brief Switches the active mini-game to the specified ID.
 * @param game_id The ID of the mini-game to switch to.
 */
void switch_minigame(u8 game_id) {
    if (game_id == 0) {
        active_game_id = 0;
        clientState = CLIENT_STATE_LOBBY;
    } else if (game_registry[game_id]) {
        active_game_id = game_id;
        clientState = CLIENT_STATE_INGAME;
        printf("[SYSTEM] Switch vers module ID:%d\n", game_id);
    }
}

/**
 * @brief Registers a mini-game module in the registry and initializes it.
 * @param module Pointer to the mini-game module structure.
 */
void register_minigame(MiniGameModule* module) {
    if (module) {
        game_registry[module->id] = module;
        if (module->init) module->init();
        printf("[SYSTEM] Module '%s' (ID:%d) enregistré.\n", module->name, module->id);
    }
}

/**
 * @brief Ensures that the network socket exists, creating it if necessary.
 */
void ensure_socket_exists(void) {
    if (network_socket != -1) return;
    network_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (network_socket >= 0) {
        s32 brd = 1;
        setsockopt(network_socket, SOL_SOCKET, SO_BROADCAST, &brd, sizeof(brd));
        fcntl(network_socket, F_SETFL, O_NONBLOCK);
    }
}

/**
 * @brief Broadcasts a query to discover available lobby servers.
 */
void discover_servers(void) {
    ensure_socket_exists();
    struct sockaddr_in addr = { .sin_family = AF_INET, .sin_port = htons(8080), .sin_addr.s_addr = INADDR_BROADCAST };
    RUDP_Header q = { .action = LOBBY_ROOM_QUERY };
    sendto(network_socket, &q, sizeof(RUDP_Header), 0, (struct sockaddr*)&addr, sizeof(addr));
}

/**
 * @brief Initializes the network connection to a target IP.
 * @param target_ip The IP address of the server to connect to.
 */
void init_network(const char* target_ip) {
    ensure_socket_exists();
    struct sockaddr_in addr = { .sin_family = AF_INET, .sin_port = htons(8080) };
    inet_pton(AF_INET, target_ip, &addr.sin_addr);
    connect(network_socket, (struct sockaddr *)&addr, sizeof(addr));
    RUDP_InitConnection(&server_conn);
    
    RUDP_Header h; RUDP_GenerateHeader(&server_conn, LOBBY_JOIN, &h);
    send(network_socket, &h, sizeof(h), 0);
}

/**
 * @brief Receives and processes incoming network data packets.
 */
void receive_network_data(LobbyGame_St* game) {
    if (network_socket == -1) return;
    u8 buffer[2048];
    struct sockaddr_in from; socklen_t len = sizeof(from);
    
    Player_st* others = lobby_getOtherPlayers(game);

    while (1) {
        ssize_t r = recvfrom(network_socket, buffer, sizeof(buffer), 0, (struct sockaddr*)&from, &len);
        if (r < (ssize_t)sizeof(RUDP_Header)) break;

        RUDP_Header* h = (RUDP_Header*)buffer;

        if (h->action == LOBBY_ROOM_INFO) {
            AddDiscoveredRoom(inet_ntoa(from.sin_addr), (char*)(buffer + sizeof(RUDP_Header)));
            continue;
        }

        if (!RUDP_ProcessIncoming(&server_conn, h)) continue;

        s32 sid = ntohs(h->sender_id);
        
        if (h->action == LOBBY_JOIN) {
            u16 temp_id;
            memcpy(&temp_id, buffer + sizeof(RUDP_Header), sizeof(u16)); 
            my_id = temp_id;
            printf("[SYSTEM] Connecté au lobby. Mon ID: %d\n", my_id);
            clientState = CLIENT_STATE_LOBBY;
        }
        else if (h->action == LOBBY_SWITCH_GAME) {
            u8 target_game_id = *(u8*)(buffer + sizeof(RUDP_Header));
            switch_minigame(target_game_id);
        }
        else if (h->action == LOBBY_MOVE) {
            if (sid == my_id) continue;
            if (sid >= 0 && sid < 8 && others) {
                Player_st incoming;
                memcpy(&incoming, buffer + sizeof(RUDP_Header), sizeof(Player_st));
                
                // If it's the first time we see this player, teleport them
                if (!others[sid].active) {
                    others[sid] = incoming;
                    others[sid].targetPosition = incoming.position;
                } else {
                    // Update all fields EXCEPT current position
                    Vector2 currentPos = others[sid].position;
                    others[sid] = incoming;
                    others[sid].position = currentPos;
                    others[sid].targetPosition = incoming.position;
                }
                others[sid].active = true;
            }
        }
        else if (h->action == LOBBY_LEAVE) {
            if (sid >= 0 && sid < 8 && others) others[sid].active = false;
        }
        else if (h->action == 5 /* ACTION_GAME_DATA / LOBBY_CHAT */) {
            GameTLVHeader* g = (GameTLVHeader*)(buffer + sizeof(RUDP_Header));
            void* payload = (u8*)g + sizeof(GameTLVHeader);
            
            // Si c'est du chat pour ma room actuelle, on l'affiche
            if (g->action == LOBBY_CHAT && g->game_id == active_game_id) {
                // Éviter d'afficher ses propres messages (déjà géré localement dans chat.c)
                if (sid != my_id) {
                    const char* sender_name = (sid == 999) ? "SERVEUR" : TextFormat("Joueur %d", sid);
                    AddChatMessage(sender_name, (char*)payload);
                }
            }

            // Dispatch au module (pour gestion spécifique si besoin)
            if (game_registry[g->game_id]) {
                game_registry[g->game_id]->on_data(sid, g->action, payload, g->length);
            }
        }
    }
}

#include "firstparty/leaderboard.h"

s32 main(void) {
    g_progress = LoadProgress();
    
    // Update AP tiers based on current leaderboard placements
    for (int g = 1; g < MAX_GAMES_PROGRESS; g++) {
        Leaderboard_St lb = LoadLeaderboard(g);
        g_progress.total_players[g][0][0] = lb.count;
        g_progress.current_ap[g] = CalculateAPTier(g_progress.leaderboard_rank[g][0][0], lb.count);
    }
    
    InitMenus();
    
    LobbyGame_St* game = NULL;
    if (lobby_initGame(&game) == ERROR_ALLOC) {
        fprintf(stderr, "Failed to init lobby\n");
        return 1;
    }

    InitConnectionScreen();
    Player_st* others = lobby_getOtherPlayers(game);
    if (others) {
        for (s32 i = 0; i < 8; i++) others[i].active = false;
    }

    register_minigame(&KingForFourClientModule);

    f32 move_timer = 0;

    while (!WindowShouldClose()) {
        f32 dt = GetFrameTime();
        if (dt > 0.1f) dt = 0.1f;
        receive_network_data(game);

        UpdateMenu();
        if (g_currentMenu != MENU_NONE) {
            // If menu is active, we might want to skip some updates
            if (g_currentMenu == MENU_PAUSE && IsKeyPressed(KEY_ESCAPE)) g_currentMenu = MENU_NONE;
        }

        if (g_currentMenu == MENU_NONE) {
            // Global chat input processing
            UpdateChat();

            Player_st* local_p = lobby_getLocalPlayer(game);

            if (clientState == CLIENT_STATE_LOBBY && active_game_id == 0 && local_p) {
                // Lobby network sync (send pos) - Throttled at 60Hz
                move_timer += dt;
                static Vector2 lastSent = {0};
                if (move_timer >= 0.016f && (local_p->position.x != lastSent.x || local_p->position.y != lastSent.y)) {
                    RUDP_Header h; RUDP_GenerateHeader(&server_conn, LOBBY_MOVE, &h);
                    h.sender_id = htons((u16)my_id);
                    u8 buf[1024];
                    memcpy(buf, &h, sizeof(h));
                    memcpy(buf + sizeof(h), local_p, sizeof(Player_st));
                    send(network_socket, buf, sizeof(h) + sizeof(Player_st), 0);
                    lastSent = local_p->position;
                    move_timer = 0;
                }

                // Game trigger check
                if (checkGameTrigger(local_p)) {
                    DrawText("APPUYEZ SUR ENTRÉE POUR JOUER", GetScreenWidth()/2 - MeasureText("APPUYEZ SUR ENTRÉE POUR JOUER", 20)/2, GetScreenHeight() - 100, 20, GOLD);
                    
                    if (IsKeyPressed(KEY_ENTER)) {
                        u8 target_id = 1; // King For Four
                        RUDP_Header h; RUDP_GenerateHeader(&server_conn, LOBBY_SWITCH_GAME, &h);
                        h.sender_id = htons((u16)my_id);
                        u8 buf[64];
                        memcpy(buf, &h, sizeof(h));
                        buf[sizeof(h)] = target_id;
                        send(network_socket, buf, sizeof(h) + 1, 0);
                        clientState = CLIENT_STATE_WAITING_SWITCH; // Avoid spamming
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
            switch (clientState) {
                case CLIENT_STATE_DISCONNECTED:
                    if (UpdateConnectionScreen()) {
                        init_network(GetEnteredIP());
                        clientState = CLIENT_STATE_CONNECTING;
                    }
                    DrawConnectionScreen();
                    break;

                case CLIENT_STATE_CONNECTING:
                    DrawText("CONNEXION EN COURS...", 100, 100, 30, GRAY);
                    break;

                case CLIENT_STATE_LOBBY:
                case CLIENT_STATE_WAITING_SWITCH:
                    lobby_gameLoop(game);
                    if (clientState == CLIENT_STATE_WAITING_SWITCH) {
                        DrawText("CHARGEMENT DU JEU...", GetScreenWidth()/2 - 100, GetScreenHeight()/2, 20, WHITE);
                    }
                    break;

                case CLIENT_STATE_INGAME:
                    if (game_registry[active_game_id]) {
                        game_registry[active_game_id]->update(dt);
                        // Check again in case update() changed the active_game_id or clientState
                        if (clientState == CLIENT_STATE_INGAME && game_registry[active_game_id]) {
                            game_registry[active_game_id]->draw();
                        }
                    }
                    break;
            }

            if (g_currentMenu == MENU_PAUSE) DrawMenu();

            // Global chat rendering overlay
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
