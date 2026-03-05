/**
 * @file main.c
 * @brief Noyau du client : Gestion des sockets, Discovery et Dispatcher de modules.
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

#define SERVER_PORT 8080
#define ACTION_GAME_DATA 5 

// --- MACHINE À ÉTATS (FIX : Était manquant) ---
typedef enum { 
    STATE_CONNECTION, 
    STATE_LOBBY 
} GameState;

static GameState currentState = STATE_CONNECTION;

// --- DÉFINITIONS GLOBALES (Accessibles par les modules via extern) ---
int network_socket = -1;
RUDP_Connection server_conn;
Player_st player = { .position = { 400, 300 }, .radius = 20, .active = true };
Player_st otherPlayers[MAX_CLIENTS];

// --- GESTION DES MODULES ---
static MiniGameModule* game_registry[256] = {0};
static uint8_t active_game_id = 0; 

// Déclaration du module défini dans lobby_module.c
extern MiniGameModule LobbyModule; 

// --- FONCTIONS SYSTÈME ---

void register_minigame(MiniGameModule* module) {
    if (module && module->id < 256) {
        game_registry[module->id] = module;
        if (module->init) module->init();
        printf("[SYSTEM] Module '%s' (ID:%d) enregistré.\n", module->name, module->id);
    }
}

void ensure_socket_exists() {
    if (network_socket != -1) return;
    network_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (network_socket >= 0) {
        int brd = 1;
        setsockopt(network_socket, SOL_SOCKET, SO_BROADCAST, &brd, sizeof(brd));
        fcntl(network_socket, F_SETFL, O_NONBLOCK);
    }
}

void discover_servers(void) {
    ensure_socket_exists();
    struct sockaddr_in addr = { .sin_family = AF_INET, .sin_port = htons(SERVER_PORT), .sin_addr.s_addr = INADDR_BROADCAST };
    RUDP_Header q = { .action = LOBBY_ROOM_QUERY };
    sendto(network_socket, &q, sizeof(RUDP_Header), 0, (struct sockaddr*)&addr, sizeof(addr));
}

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

        if (h->action == ACTION_GAME_DATA && RUDP_ProcessIncoming(&server_conn, h)) {
            GameTLVHeader* g = (GameTLVHeader*)(buffer + sizeof(RUDP_Header));
            void* payload = (uint8_t*)g + sizeof(GameTLVHeader);
            
            if (game_registry[g->game_id]) {
                game_registry[g->game_id]->on_data(ntohs(h->sender_id), g->action, payload, g->length);
            }
        }
    }
}

// --- BOUCLE PRINCIPALE ---

int main(void) {
    InitWindow(1280, 720, "Multi-Mini-Games");
    SetTargetFPS(60);

    InitConnectionScreen();
    
    for(int i = 0; i < MAX_CLIENTS; i++) otherPlayers[i].active = false;
    register_minigame(&LobbyModule);

    while (!WindowShouldClose()) {
        float dt = GetFrameTime();
        receive_network_data();

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