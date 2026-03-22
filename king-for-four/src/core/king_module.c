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
#include "game_interface.h"
#include "core/protocol.h"
#include "APIs/generalAPI.h"

/**
 * @struct KingServerState
 * @brief Internal state maintained by the server for a game instance.
 */
typedef struct {
    GameState game;         /**< Core game logic state */
    s32 player_ids[4];      /**< Network IDs of the 4 players (human or -1 for bot) */
    s32 human_count;        /**< Number of human players joined */
    s32 status;             /**< 0: WAITING, 1: PLAYING, 2: GAME_OVER */
    s32 winner_id;          /**< Network ID of the winner */
    f32 bot_timer;          /**< Timer for bot thinking time */
} KingServerState;

/**
 * @brief Initializes a game instance on the server.
 */
void* king_create_instance(void) {
    KingServerState* ks = calloc(1, sizeof(KingServerState));
    if (ks) {
        init_game_logic(&ks->game);
        for (s32 i = 0; i < 4; i++) ks->player_ids[i] = -1;
        ks->human_count = 0;
        ks->status = 0;
        ks->winner_id = -1;
        ks->bot_timer = 0;
    }
    return ks;
}

/**
 * @brief Helper to wrap game data in ACTION_GAME_DATA (5) with a GameTLVHeader.
 */
static void server_send_game_data(s32 target_id, s32 sender_id, u8 action, void* data, u16 len, broadcast_func_t broadcast) {
    GameTLVHeader tlv = { .game_id = 1, .action = action, .length = len };
    u8 buffer[2048];
    memcpy(buffer, &tlv, sizeof(tlv));
    if (len > 0 && data) memcpy(buffer + sizeof(tlv), data, len);
    broadcast(target_id, sender_id, 5 /* ACTION_GAME_DATA */, buffer, (u16)(sizeof(tlv) + len));
}

/**
 * @brief Synchronizes the game state with all connected clients.
 */
static void broadcast_sync(KingServerState* ks, broadcast_func_t broadcast) {
    GameSyncPayload sync = {
        .current_player = ks->game.current_player,
        .active_color = ks->game.active_color,
        .status = ks->status,
        .host_id = ks->player_ids[0],
        .winner_id = ks->winner_id
    };
    
    if (ks->game.discard_pile.head) {
        sync.top_card = ks->game.discard_pile.head->card;
    }
    
    for (s32 i = 0; i < 4; i++) {
        sync.hand_sizes[i] = ks->game.players[i].hand.size;
    }
    
    // Broadcast sync to all in room
    server_send_game_data(-1, 999, ACTION_SYNC_GAME, &sync, sizeof(sync), broadcast);

    for (s32 i = 0; i < ks->game.num_players; i++) {
        s32 player_id = ks->player_ids[i];
        if (player_id < 0) continue; // Skip bots for hand sync
        
        s32 hand_count = ks->game.players[i].hand.size;
        if (hand_count == 0) continue;

        Card* hand_data = malloc(hand_count * sizeof(Card));
        Node* curr = ks->game.players[i].hand.head;
        for (s32 j = 0; j < hand_count && curr; j++) {
            hand_data[j] = curr->card;
            curr = curr->next;
        }
        
        // Unicast hand sync to player
        server_send_game_data(player_id, 999, ACTION_SYNC_HAND, hand_data, (u16)(hand_count * sizeof(Card)), broadcast);
        free(hand_data);
    }
}

/**
 * @brief Processes client actions and broadcasts updates.
 */
void king_on_action(void *state, s32 player_id, u8 action, void *payload, u16 len, broadcast_func_t broadcast) {
    KingServerState* ks = (KingServerState*)state;
    if (action != 5) return;

    if (len < sizeof(GameTLVHeader)) return;
    GameTLVHeader* tlv = (GameTLVHeader*)payload;
    if (tlv->game_id != 1) return;
    
    u8 real_action = tlv->action;
    void* real_payload = (u8*)payload + sizeof(GameTLVHeader);

    s32 internal_id = -1;
    for (s32 i = 0; i < ks->game.num_players; i++) {
        if (ks->player_ids[i] == player_id) {
            internal_id = i;
            break;
        }
    }

    if (real_action == ACTION_JOIN_GAME) {
        if (ks->human_count < 4 && ks->status == 0 && internal_id == -1) {
            s32 idx = ks->game.num_players++;
            ks->player_ids[idx] = player_id;
            ks->human_count++;
            init_player(&ks->game.players[idx], idx, "Player");
            printf("[KING] Joueur %d rejoint au slot %d\n", player_id, idx);
            
            // Unicast ACK to the joining player
            server_send_game_data(player_id, 999, ACTION_JOIN_ACK, &idx, sizeof(s32), broadcast);
            broadcast_sync(ks, broadcast);
        }
    }
    else if (real_action == ACTION_START_GAME && ks->status == 0 && internal_id == 0) {
        s32 total_requested = 4;
        if (tlv->length >= sizeof(s32)) memcpy(&total_requested, real_payload, sizeof(s32));
        if (total_requested < ks->game.num_players) total_requested = ks->game.num_players;
        if (total_requested > 4) total_requested = 4;

        while (ks->game.num_players < total_requested) {
            s32 bot_idx = ks->game.num_players++;
            ks->player_ids[bot_idx] = -1; // Bot ID
            init_player(&ks->game.players[bot_idx], bot_idx, "Bot");
            printf("[KING] Bot ajouté au slot %d\n", bot_idx);
        }

        ks->status = 1;
        init_uno_deck(&ks->game.draw_pile);
        human_shuffle_deck(&ks->game.draw_pile);
        distribute_cards(&ks->game);
        printf("[KING] Partie lancée avec %d joueurs\n", ks->game.num_players);
        broadcast_sync(ks, broadcast);
    }
    else if (real_action == ACTION_PLAY_CARD && ks->status == 1 && internal_id == ks->game.current_player) {
        ActionPlayPayload_St* p = (ActionPlayPayload_St*)real_payload;
        if (try_play_card(&ks->game, internal_id, p->card_index)) {
            if (ks->game.discard_pile.head->card.color == CARD_BLACK) {
                ks->game.active_color = p->chosen_color;
            } else {
                ks->game.active_color = -1;
            }

            // Check for victory
            if (ks->game.players[internal_id].hand.size == 0) {
                printf("[KING] Victoire du Joueur %d !\n", internal_id);
                ks->status = 2; // GAME_OVER
                ks->winner_id = internal_id;
            } else {
                ks->game.current_player = (ks->game.current_player + ks->game.game_direction + ks->game.num_players) % ks->game.num_players;
            }
            broadcast_sync(ks, broadcast);
        }
    }
    else if (real_action == ACTION_DRAW_CARD && ks->status == 1 && internal_id == ks->game.current_player) {
        player_draw_card(&ks->game, internal_id);
        ks->game.current_player = (ks->game.current_player + ks->game.game_direction + ks->game.num_players) % ks->game.num_players;
        broadcast_sync(ks, broadcast);
    }
    else if (real_action == ACTION_QUIT_GAME) {
        // Géré par on_player_leave
    }
}

/**
 * @brief Updates the server logic (bots, timers).
 */
void king_on_tick(void *state, broadcast_func_t broadcast) {
    KingServerState* ks = (KingServerState*)state;
    if (ks->status != 1) return;

    if (ks->player_ids[ks->game.current_player] == -1) { // It's a bot's turn
        ks->bot_timer += 0.016f; // Assume 60Hz tick from server.c
        if (ks->bot_timer > 1.5f) {
            s32 bot_idx = ks->game.current_player;
            s32 card_idx = -1;
            calculate_best_move(&ks->game, bot_idx, &card_idx);
            
            if (card_idx != -1) {
                // Determine if it's a black card to choose color
                Node* curr = ks->game.players[bot_idx].hand.head;
                for(s32 i=0; i<card_idx; i++) curr = curr->next;
                
                if (curr) {
                    Card toPlay = curr->card; // Capture data BEFORE freeing the node
                    if (try_play_card(&ks->game, bot_idx, card_idx)) {
                        if (toPlay.color == CARD_BLACK) {
                            ks->game.active_color = rand() % 4;
                        }
                        
                        // Check for victory
                        if (ks->game.players[bot_idx].hand.size == 0) {
                            printf("[KING] Victoire du Bot %d !\n", bot_idx);
                            ks->status = 2; // GAME_OVER
                            ks->winner_id = bot_idx;
                        } else {
                            ks->game.current_player = (ks->game.current_player + ks->game.game_direction + ks->game.num_players) % ks->game.num_players;
                        }
                    }
                }
            } else {
                player_draw_card(&ks->game, bot_idx);
                ks->game.current_player = (ks->game.current_player + ks->game.game_direction + ks->game.num_players) % ks->game.num_players;
            }
            ks->bot_timer = 0;
            broadcast_sync(ks, broadcast);
        }
    }
}

/**
 * @brief Handles player departure.
 */
void king_on_player_leave(void *state, s32 player_id) {
    KingServerState* ks = (KingServerState*)state;
    for (s32 i = 0; i < ks->game.num_players; i++) {
        if (ks->player_ids[i] == player_id) {
            ks->player_ids[i] = -1; // Transform into bot
            printf("[KING] Joueur %d remplacé par un bot au slot %d\n", player_id, i);
            break;
        }
    }
}

/**
 * @brief Cleans up the game instance.
 */
void king_destroy_instance(void *state) {
    KingServerState* ks = (KingServerState*)state;
    if (ks) {
        for (s32 i = 0; i < 4; i++) clear_deck(&ks->game.players[i].hand);
        clear_deck(&ks->game.draw_pile);
        clear_deck(&ks->game.discard_pile);
        free(ks);
    }
}

GameInterface king_module = {
    .game_name = "king-for-four",
    .create_instance = king_create_instance,
    .on_action = king_on_action,
    .on_tick = king_on_tick, 
    .on_player_leave = king_on_player_leave,
    .destroy_instance = king_destroy_instance
};
