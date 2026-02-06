// games/lobby.c
#include "protocol.h"
#include "game_interface.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// --- STRUCTURES INTERNES AU LOBBY ---
typedef struct {
    float x, y;
    int is_active;
} Penguin;

typedef struct {
    Penguin players[100]; // Max 100 joueurs dans le lobby
} LobbyState;

// --- LOGIQUE DU JEU ---

void* lobby_create() {
    printf("[Lobby] Création du monde ouvert...\n");
    LobbyState *world = malloc(sizeof(LobbyState));
    memset(world, 0, sizeof(LobbyState));
    return (void*)world;
}

void lobby_on_action(void *state, int player_id, uint8_t action, void *payload, uint16_t len) {
    LobbyState *world = (LobbyState*)state;

    switch (action) {
        case LOBBY_JOIN:
            printf("[Lobby] Joueur %d a rejoint le QG.\n", player_id);
            world->players[player_id].is_active = 1;
            world->players[player_id].x = 50.0f; // Spawn au milieu
            world->players[player_id].y = 50.0f;
            break;

        case LOBBY_MOVE:
            if (len == sizeof(LobbyMovePayload)) {
                LobbyMovePayload *move = (LobbyMovePayload*)payload;
                world->players[player_id].x = move->x;
                world->players[player_id].y = move->y;
                // Ici, tu pourrais renvoyer la nouvelle position à TOUS les autres joueurs (Broadcast)
                // printf("[Lobby] Joueur %d bouge vers %.2f, %.2f\n", player_id, move->x, move->y);
            }
            break;
    }
}

// C'est ici qu'on gère les "Zones" (Triggers)
void lobby_tick(void *state) {
    LobbyState *world = (LobbyState*)state;
    
    // Zone de lancement du SNAKE (ex: x=100, y=100)
    for (int i = 0; i < 100; i++) {
        if (world->players[i].is_active) {
            float px = world->players[i].x;
            float py = world->players[i].y;

            // Collision simple (Carré entre 90 et 110)
            if (px > 90 && px < 110 && py > 90 && py < 110) {
                printf("[Lobby] Le joueur %d marche sur le tapis SNAKE !\n", i);
                // TODO: Envoyer paquet "GameSwitchPayload" au client
                // world->players[i].x = 0; // Reset position pour pas re-déclencher
            }
        }
    }
}

// --- EXPORT DU MODULE ---
GameInterface lobby_module = {
    .game_name = "lobby",
    .create_instance = lobby_create,
    .on_action = lobby_on_action,
    .on_tick = lobby_tick,
    .on_player_leave = NULL,
    .destroy_instance = free
};