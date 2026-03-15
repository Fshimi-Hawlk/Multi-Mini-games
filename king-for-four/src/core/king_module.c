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

#define ACTION_PLAY_CARD 0x10
#define ACTION_DRAW_CARD 0x11
#define ACTION_SYNC_GAME 0x12
#define ACTION_JOIN_GAME 0x13
#define ACTION_START_GAME 0x14
#define ACTION_SYNC_HAND 0x15

/**
 * @struct GameSyncPayload
 * @brief État condensé envoyé aux clients pour affichage.
 */
#pragma pack(push, 1)
typedef struct {
    int current_player;
    int active_color;
    Card top_card;
    int hand_sizes[4];
} GameSyncPayload;
#pragma pack(pop)

/** @brief Initialise une partie sur le serveur. */
void* king_create_instance() {
    GameState* state = calloc(1, sizeof(GameState));
    if (state) {
        init_game_logic(state);
        init_uno_deck(&state->draw_pile);
        shuffle_deck(&state->draw_pile);
    }
    return state;
}

/** @brief Traite les clics/actions des clients. */
void king_on_action(void *state, int player_id, uint8_t action, void *payload, uint16_t len, broadcast_func_t broadcast) {
    GameState* g = (GameState*)state;

    // 1. GESTION DES CONNEXIONS
    int internal_id = -1;
    for (int i = 0; i < g->num_players; i++) {
        if (g->players[i].id == player_id) {
            internal_id = i;
            break;
        }
    }

    if (internal_id == -1) {
        if (g->num_players < 4) {
            internal_id = g->num_players++;
            init_player(&g->players[internal_id], player_id, "Joueur");
            printf("[KING] Nouveau joueur enregistré: %d (Slot %d)\n", player_id, internal_id);
        } else {
            return; // Partie pleine
        }
    }

    // 2. ACTIONS RÉSEAU
    if (action == ACTION_START_GAME && internal_id == 0) {
        distribute_cards(g);
        printf("[KING] Partie démarrée par l'hôte.\n");
    }
    else if (action == ACTION_PLAY_CARD && internal_id == g->current_player) {
        int card_index = *((int*)payload);
        if (try_play_card(g, internal_id, card_index)) {
            g->current_player = (g->current_player + g->game_direction + g->num_players) % g->num_players;
        }
    } else if (action == ACTION_DRAW_CARD && internal_id == g->current_player) {
        player_draw_card(g, internal_id);
        g->current_player = (g->current_player + g->game_direction + g->num_players) % g->num_players;
    }

    // 3. SYNCHRONISATION
    Card top_card = {CARD_BLACK, ZERO};
    if (g->discard_pile.head != NULL) {
        top_card = g->discard_pile.head->card;
    }

    GameSyncPayload sync;
    sync.current_player = g->current_player;
    sync.active_color = g->active_color;
    sync.top_card = top_card;
    for (int i = 0; i < 4; i++) {
        sync.hand_sizes[i] = (i < g->num_players) ? g->players[i].hand.size : 0;
    }

    // A. Broadcast de l'état global
    broadcast(0, -1, ACTION_SYNC_GAME, &sync, sizeof(GameSyncPayload));
    
    // B. Unicast des mains (Chaque joueur reçoit sa main uniquement)
    for (int i = 0; i < g->num_players; i++) {
        int target_player_id = g->players[i].id;
        int hand_count = g->players[i].hand.size;
        Card* cards = malloc(hand_count * sizeof(Card));
        
        Node* curr = g->players[i].hand.head;
        for (int j = 0; j < hand_count; j++) {
            cards[j] = curr->card;
            curr = curr->next;
        }
        
        // room_id = -1 -> UNICAST vers target_player_id
        broadcast(-1, target_player_id, ACTION_SYNC_HAND, cards, hand_count * sizeof(Card));
        free(cards);
    }
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