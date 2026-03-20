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

void* lobby_create() {
    LobbyState* state = (LobbyState*)malloc(sizeof(LobbyState));
    if (state) {
        memset(state, 0, sizeof(LobbyState));
    }
    return state;
}

void lobby_on_action(void *state, int player_id, uint8_t action, void *payload, uint16_t len, broadcast_func_t broadcast) {
    (void)state;
    // Relay everything back to all clients (target_id = -1, original sender_id = player_id)
    broadcast(-1, player_id, action, payload, len);
}

void lobby_on_player_leave(void *state, int player_id) {
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
