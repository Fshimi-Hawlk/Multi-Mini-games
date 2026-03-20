/**
 * @file king_module.c
 * @author i-Charlys (CAILLON Charles)
 * @date 2026-03-18
 * @brief Server-side module for the King-for-Four (Uno) game.
 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "core/game.h"
#include "core/card.h"
#include "core/player.h"
#include "core/bot.h"
#include "core/protocol.h"
#include "game_interface.h"
#include "APIs/generalAPI.h"

/**
 * @struct KingServerState
 * @brief Complete state of a King-for-Four game instance on the server.
 */
typedef struct {
    GameState game;         /**< Uno game logic state */
    int player_count;       /**< Current number of players */
    int player_ids[4];      /**< Network IDs of connected players */
    int status;             /**< 0: WAITING, 1: PLAYING */
    float bot_timer;        /**< Timer for bot actions */
} KingServerState;

/**
 * @brief Synchronizes the game state with all connected clients.
 * @param ks Pointer to the server state.
 * @param broadcast Function pointer for broadcasting data.
 */
static void broadcast_sync(KingServerState* ks, broadcast_func_t broadcast) {
    GameSyncPayload sync = {
        .current_player = ks->game.current_player,
        .active_color = ks->game.active_color,
        .status = ks->status,
        .host_id = ks->player_ids[0]
    };
    
    if (ks->game.discard_pile.head) {
        sync.top_card = ks->game.discard_pile.head->card;
    }
    
    for (int i = 0; i < 4; i++) {
        sync.hand_sizes[i] = ks->game.players[i].hand.size;
    }
    
    // Broadcast sync to all (target_id = -1, sender_id = 0)
    broadcast(-1, 0, ACTION_SYNC_GAME, &sync, sizeof(sync));

    for (int i = 0; i < ks->player_count; i++) {
        int player_id = ks->player_ids[i];
        if (player_id == -1) continue;
        
        int hand_count = ks->game.players[i].hand.size;
        if (hand_count == 0) continue;

        Card* hand_data = malloc(hand_count * sizeof(Card));
        Node* curr = ks->game.players[i].hand.head;
        for (int j = 0; j < hand_count && curr; j++) {
            hand_data[j] = curr->card;
            curr = curr->next;
        }
        
        // Unicast hand sync to player
        broadcast(player_id, 0, ACTION_SYNC_HAND, hand_data, hand_count * sizeof(Card));
        free(hand_data);
    }
}

/**
 * @brief Creates a new game instance.
 * @return Pointer to the allocated state.
 */
void* king_create_instance(void) {
     KingServerState* ks = calloc(1, sizeof(KingServerState));
    init_game_logic(&ks->game);
    for (int i = 0; i < 4; i++) ks->player_ids[i] = -1;
    ks->status = 0;
    ks->player_count = 0;
    return ks;
}

/**
 * @brief Handles an action from a player.
 */
void king_on_action(void *state, int player_id, uint8_t action, void *payload, uint16_t len, broadcast_func_t broadcast) {
    KingServerState* ks = (KingServerState*)state;
    (void)action; // Action general is ACTION_GAME_DATA handled by server

    if (len < sizeof(GameTLVHeader)) return;
    GameTLVHeader* tlv = (GameTLVHeader*)payload;
    if (tlv->game_id != 1) return; 
    
    uint8_t real_action = tlv->action;
    void* real_payload = (uint8_t*)payload + sizeof(GameTLVHeader);
    uint16_t real_len = tlv->length;

    if (real_action == ACTION_JOIN_GAME) {
        if (ks->player_count < 4 && ks->status == 0) {
            ks->player_ids[ks->player_count] = player_id;
            init_player(&ks->game.players[ks->player_count], ks->player_count, "Player");
            
            // Unicast ACK to the joining player
            broadcast(player_id, 0, ACTION_JOIN_ACK, &ks->player_count, sizeof(int));
            ks->player_count++;
            broadcast_sync(ks, broadcast);
        }
    }
    else if (real_action == ACTION_START_GAME && ks->status == 0 && player_id == ks->player_ids[0]) {
        ks->status = 1;
        init_uno_deck(&ks->game.draw_pile);
        human_shuffle_deck(&ks->game.draw_pile);
        distribute_cards(&ks->game);
        broadcast_sync(ks, broadcast);
    }
    else if (real_action == ACTION_PLAY_CARD && ks->status == 1) {
        int internal_id = -1;
        for (int i = 0; i < 4; i++) if (ks->player_ids[i] == player_id) internal_id = i;
        
        if (internal_id == ks->game.current_player && real_len >= sizeof(ActionPlayPayload_St)) {
            ActionPlayPayload_St* p = (ActionPlayPayload_St*)real_payload;
            if (try_play_card(&ks->game, internal_id, p->card_index)) {
                if (p->chosen_color != -1) ks->game.active_color = p->chosen_color;
                broadcast_sync(ks, broadcast);
            }
        }
    }
    else if (real_action == ACTION_DRAW_CARD && ks->status == 1) {
        int internal_id = -1;
        for (int i = 0; i < 4; i++) if (ks->player_ids[i] == player_id) internal_id = i;
        if (internal_id == ks->game.current_player) {
            player_draw_card(&ks->game, internal_id);
            broadcast_sync(ks, broadcast);
        }
    }
}

/**
 * @brief Updates the server logic (bots, timers).
 */
void king_on_tick(void *state, broadcast_func_t broadcast) {
    KingServerState* ks = (KingServerState*)state;
    if (ks->status != 1) return;

    ks->bot_timer += 0.016f;
    if (ks->bot_timer > 2.0f) {
        if (ks->game.current_player >= ks->player_count) {
            int bot_idx = ks->game.current_player;
            int card_idx = -1;
            calculate_best_move(&ks->game, bot_idx, &card_idx);
            if (card_idx != -1) {
                try_play_card(&ks->game, bot_idx, card_idx);
            } else {
                player_draw_card(&ks->game, bot_idx);
            }
            broadcast_sync(ks, broadcast);
        }
        ks->bot_timer = 0;
    }
}

/**
 * @brief Handles player departure.
 */
void king_on_player_leave(void *state, int player_id) {
    KingServerState* ks = (KingServerState*)state;
    for (int i = 0; i < 4; i++) {
        if (ks->player_ids[i] == player_id) {
            ks->player_ids[i] = -1;
        }
    }
}

/**
 * @brief Destroys the game instance.
 */
void king_destroy_instance(void *state) {
    KingServerState* ks = (KingServerState*)state;
    free_deck(&ks->game.draw_pile);
    free_deck(&ks->game.discard_pile);
    for (int i = 0; i < 4; i++) free_deck(&ks->game.players[i].hand);
    free(ks);
}

/** @brief Global definition of the King-for-Four server module. */
GameInterface king_module = {
    .game_name = "king-for-four",
    .create_instance = king_create_instance,
    .on_action = king_on_action,
    .on_tick = king_on_tick, 
    .on_player_leave = king_on_player_leave,
    .destroy_instance = king_destroy_instance
};
