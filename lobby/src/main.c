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
#include "rudp_core.h"
#include "utils/globals.h"
#include <arpa/inet.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>

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
int network_socket = -1;

/** @brief RUDP connection state for the server. */
RUDP_Connection server_conn;

/** @brief Registry of available mini-game modules. */
static MiniGameModule* game_registry[256] = {0};

/** @brief ID of the currently active mini-game module. */
uint8_t active_game_id = 0;

/** @brief Local player ID assigned by server. */
static int my_id = -1;

extern MiniGameModule KingForFourClientModule;

/**
 * @brief Switches the active mini-game to the specified ID.
 * @param game_id The ID of the mini-game to switch to.
 */
void switch_minigame(uint8_t game_id) {
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
    
    // Join Lobby
    RUDP_Header h; RUDP_GenerateHeader(&server_conn, LOBBY_JOIN, &h);
    // On ne connaît pas encore notre ID, on laisse 0 ou 999
    send(network_socket, &h, sizeof(h), 0);
}

/**
 * @brief Receives and processes incoming network data packets.
 */
void receive_network_data(LobbyGame_St* game) {
    if (network_socket == -1) return;
    uint8_t buffer[2048];
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

        int sid = ntohs(h->sender_id);
        
        if (h->action == LOBBY_JOIN) {
            my_id = *(uint16_t*)(buffer + sizeof(RUDP_Header));
            printf("[SYSTEM] Connecté au lobby. Mon ID: %d\n", my_id);
            clientState = CLIENT_STATE_LOBBY;
        }
        else if (h->action == LOBBY_SWITCH_GAME) {
            uint8_t target_game_id = *(uint8_t*)(buffer + sizeof(RUDP_Header));
            switch_minigame(target_game_id);
        }
        else if (h->action == LOBBY_MOVE) {
            if (sid == my_id) continue;
            if (sid >= 0 && sid < 8 && others) {
                memcpy(&others[sid], buffer + sizeof(RUDP_Header), sizeof(Player_st));
                others[sid].active = true;
            }
        }
        else if (h->action == LOBBY_LEAVE) {
            if (sid >= 0 && sid < 8 && others) others[sid].active = false;
        }
        else if (h->action == LOBBY_CHAT) {
            int pid = ntohs(h->sender_id);
            if (active_game_id != 0 && game_registry[active_game_id]) {
                game_registry[active_game_id]->on_data(pid, LOBBY_CHAT, buffer + sizeof(RUDP_Header), r - sizeof(RUDP_Header));
            }
        }
        else if (h->action == 5 /* ACTION_GAME_DATA */) {
            GameTLVHeader* g = (GameTLVHeader*)(buffer + sizeof(RUDP_Header));
            void* payload = (uint8_t*)g + sizeof(GameTLVHeader);
            if (game_registry[g->game_id]) {
                game_registry[g->game_id]->on_data(ntohs(h->sender_id), g->action, payload, g->length);
            }
        }
    }
}

int main(void) {
    LobbyGame_St* game = NULL;
    if (lobby_initGame(&game) == ERROR_ALLOC) {
        fprintf(stderr, "Failed to init lobby\n");
        return 1;
    }

    InitConnectionScreen();
    Player_st* others = lobby_getOtherPlayers(game);
    if (others) {
        for (int i = 0; i < 8; i++) others[i].active = false;
    }

    register_minigame(&KingForFourClientModule);

    while (!WindowShouldClose()) {
        float dt = GetFrameTime();
        if (dt > 0.1f) dt = 0.1f;
        receive_network_data(game);

        Player_st* local_p = lobby_getLocalPlayer(game);

        if (clientState == CLIENT_STATE_LOBBY && active_game_id == 0 && local_p) {
            // Lobby network sync (send pos)
            static Vector2 lastSent = {0};
            if (local_p->position.x != lastSent.x || local_p->position.y != lastSent.y) {
                RUDP_Header h; RUDP_GenerateHeader(&server_conn, LOBBY_MOVE, &h);
                h.sender_id = htons((uint16_t)my_id); // CRITICAL FIX: Send our real ID
                uint8_t buf[1024];
                memcpy(buf, &h, sizeof(h));
                memcpy(buf + sizeof(h), local_p, sizeof(Player_st));
                send(network_socket, buf, sizeof(h) + sizeof(Player_st), 0);
                lastSent = local_p->position;
            }

            // Game trigger check
            if (checkGameTrigger(local_p)) {
                uint8_t target_id = 1; // King For Four
                RUDP_Header h; RUDP_GenerateHeader(&server_conn, LOBBY_SWITCH_GAME, &h);
                h.sender_id = htons((uint16_t)my_id);
                uint8_t buf[64];
                memcpy(buf, &h, sizeof(h));
                buf[sizeof(h)] = target_id;
                send(network_socket, buf, sizeof(h) + 1, 0);
                clientState = CLIENT_STATE_WAITING_SWITCH; // Avoid spamming
                printf("[SYSTEM] Demande de switch envoyée, attente confirmation...\n");
            }
        }

        BeginDrawing();
        ClearBackground(RAYWHITE);

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
                    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(BLACK, 0.3f));
                    DrawText("CHARGEMENT DU JEU...", GetScreenWidth()/2 - 100, GetScreenHeight()/2, 20, WHITE);
                }
                break;

            case CLIENT_STATE_INGAME:
                if (game_registry[active_game_id]) {
                    game_registry[active_game_id]->update(dt);
                    game_registry[active_game_id]->draw();
                }
                break;
        }
        
        EndDrawing();
    }

    lobby_freeGame(&game);
    return 0;
}

#define LOGGER_IMPLEMENTATION
#include "logger.h"
#define SYSTEM_SETTINGS_IMPLEMENTATION
#include "systemSettings.h"
