/**
 * @file lobby.c
 * @brief Implémentation de la logique de gestion du lobby.
 */

#include <stdlib.h> 
#include <string.h> 
#include "rudp_core.h"           
#include "game_interface.h" 
#include "../../lobby/include/utils/userTypes.h"

/**
 * @struct LobbyState
 * @brief État interne d'une instance de lobby.
 */
typedef struct {
    Player_st players[MAX_CLIENTS];
} LobbyState;

void* lobby_create(void) {
    LobbyState* state = (LobbyState*)malloc(sizeof(LobbyState));
    if (state) {
        memset(state, 0, sizeof(LobbyState));
    }
    return state;
}

void lobby_on_action(void *state, s32 player_id, u8 action, void *payload, u16 len, broadcast_func_t broadcast) {
    LobbyState *s = (LobbyState*)state;

    if (action == LOBBY_JOIN) {
        // Un client vient de rejoindre, on lui envoie la position de TOUS les autres
        for (s32 i = 0; i < MAX_CLIENTS; i++) {
            if (i != player_id && s->players[i].active) {
                // On simule un message LOBBY_MOVE venant du joueur i vers le nouveau client
                broadcast(player_id, i, LOBBY_MOVE, &s->players[i], sizeof(Player_st));
            }
        }
    }
    else if (action == LOBBY_MOVE) {
        // On enregistre la position du joueur
        if (player_id >= 0 && player_id < MAX_CLIENTS && len == sizeof(Player_st)) {
            memcpy(&s->players[player_id], payload, sizeof(Player_st));
            s->players[player_id].active = true;
        }
        // Relay everything back to all clients in same room (target_id = -1, original sender_id = player_id)
        broadcast(-1, player_id, action, payload, len);
    }
    else {
        broadcast(-1, player_id, action, payload, len);
    }
}

void lobby_on_player_leave(void *state, s32 player_id) {
    LobbyState *s = (LobbyState*)state;
    if (player_id >= 0 && player_id < MAX_CLIENTS) {
        memset(&s->players[player_id], 0, sizeof(Player_st));
    }
}

void lobby_tick(void *state, broadcast_func_t broadcast) {
    (void)state;
    (void)broadcast;
}

void lobby_destroy(void *state) { 
    if (state) {
        free(state); 
    }
}

GameInterface lobby_module = {
    .game_name = "lobby",
    .create_instance = lobby_create,
    .on_action = lobby_on_action,
    .on_tick = lobby_tick,
    .destroy_instance = lobby_destroy,
    .on_player_leave = lobby_on_player_leave 
};
