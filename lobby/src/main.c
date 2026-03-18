/**
 * @file main.c
 * @author i-Charlys (CAILLON Charles)
 * @date 2026-03-18
 * @brief Main entry point for the lobby client, handling networking and module dispatching.
 */

#include "raylib.h"
#include "core/game.h"
#include "ui/connection_screen.h"
#include "firstparty/APIs/module_interface.h"
#include "rudp_core.h"
#include "utils/globals.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <fcntl.h>

/** @brief Port used for server communication. */
#define SERVER_PORT 8080
/** @brief Action code for game data transmission. */
#define ACTION_GAME_DATA 5 

/**
 * @brief Enum for different game states in the lobby.
 */
typedef enum { 
    STATE_CONNECTION, ///< Initial connection state.
    STATE_LOBBY      ///< Active lobby state.
} GameState;

/** @brief Current state of the game. */
static GameState currentState = STATE_CONNECTION;

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
 * @brief Main function containing the game loop.
 * @return Exit status of the application.
 */
int main(void) {
    InitWindow(1280, 720, "Multi-Mini-Games");
    SetTargetFPS(60);

    InitConnectionScreen();
    
    for(int i = 0; i < MAX_CLIENTS; i++) otherPlayers[i].active = false;
    register_minigame(&LobbyModule);
    register_minigame(&KingForFourClientModule);

    while (!WindowShouldClose()) {
        float dt = GetFrameTime();
        if (dt > 0.1f) dt = 0.1f;
        receive_network_data();

        static bool switch_sent = false;
        if (currentState == STATE_LOBBY && active_game_id == 0) {
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
            case STATE_CONNECTION:
                static float timer = 0; timer += dt;
                if (timer > 2.0f) { discover_servers(); timer = 0; }
                if (UpdateConnectionScreen()) {
                    init_network(GetEnteredIP());
                    currentState = STATE_LOBBY;
                }
                BeginDrawing(); 
                    ClearBackground(RAYWHITE);
                    DrawConnectionScreen(); 
                EndDrawing();
                break;

            case STATE_LOBBY:
                if (game_registry[active_game_id]) {
                    game_registry[active_game_id]->update(dt);
                    BeginDrawing();
                        ClearBackground(RAYWHITE);
                        game_registry[active_game_id]->draw();
                    EndDrawing();
                }
                break;
        }
    }

    if (network_socket != -1) close(network_socket);
    CloseWindow();
    return 0;
}