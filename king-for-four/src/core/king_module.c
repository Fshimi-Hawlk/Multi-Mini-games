/**
 * @file king_module.c
 * @brief Module serveur pour le jeu King-for-Four (Uno).
 */

#include <stdlib.h>
#include <string.h>
#include "core/game.h"
#include "core/card.h"
#include "core/player.h"
#include "game_interface.h" // Interface réseau globale

// Codes d'actions spécifiques à King-for-Four
#define ACTION_PLAY_CARD 0x10
#define ACTION_DRAW_CARD 0x11
#define ACTION_SYNC_GAME 0x12

/**
 * @struct GameSyncPayload
 * @brief État condensé envoyé aux clients pour affichage.
 */
#pragma pack(push, 1)
typedef struct {
    int current_player;
    int active_color;
    Card top_card;
} GameSyncPayload;
#pragma pack(pop)

/** @brief Initialise une partie sur le serveur. */
void* king_create_instance() {
    GameState* state = malloc(sizeof(GameState));
    if (state) {
        init_game_logic(state);
        init_uno_deck(&state->draw_pile);
        shuffle_deck(&state->draw_pile);
        // On attend que les clients se connectent pour distribuer
    }
    return state;
}

/** @brief Traite les clics/actions des clients. */
void king_on_action(void *state, int player_id, uint8_t action, void *payload, uint16_t len, broadcast_func_t broadcast) {
    GameState* g = (GameState*)state;

    // Validation : Est-ce le tour de ce joueur ?
    if (g->players[g->current_player].id != player_id) return;

    if (action == ACTION_PLAY_CARD) {
        int card_index = *((int*)payload);
        if (try_play_card(g, g->current_player, card_index)) {
            // Logique de changement de tour incluse dans try_play_card
        }
    } else if (action == ACTION_DRAW_CARD) {
        player_draw_card(g, g->current_player);
    }

    // Diffusion du nouvel état à TOUS les joueurs (Room 0 par défaut)
    GameSyncPayload sync = { g->current_player, g->active_color, g->discard_pile.head->card };
    broadcast(0, -1, ACTION_SYNC_GAME, &sync, sizeof(GameSyncPayload));
}

void king_destroy_instance(void *state) {
    free(state);
}

// Exportation de l'interface pour le Linker
GameInterface king_module = {
    .game_name = "king-for-four",
    .create_instance = king_create_instance,
    .on_action = king_on_action,
    .on_tick = NULL, // Pas de logique temporelle nécessaire pour un jeu de cartes
    .on_player_leave = NULL,
    .destroy_instance = king_destroy_instance
};