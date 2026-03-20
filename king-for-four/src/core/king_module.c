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

/** @brief Action code for playing a card. */
#define ACTION_PLAY_CARD 0x10
/** @brief Action code for drawing a card. */
#define ACTION_DRAW_CARD 0x11
/** @brief Action code for synchronizing the game state. */
#define ACTION_SYNC_GAME 0x12
/** @brief Action code for joining a game. */
#define ACTION_JOIN_GAME 0x13
/** @brief Action code for starting a game. */
#define ACTION_START_GAME 0x14
/** @brief Action code for synchronizing a player's hand. */
#define ACTION_SYNC_HAND 0x15
/** @brief Action code for acknowledging a join request. */
#define ACTION_JOIN_ACK  0x16
/** @brief Action code for quitting the game. */
#define ACTION_QUIT_GAME 0x17

#pragma pack(push, 1)
/**
 * @struct GameTLVHeader_St
 * @brief Header for game messages using TLV (Type-Length-Value) format.
 */
typedef struct {
    u8 game_id;    /**< ID of the game */
    u8 action;     /**< Action type */
    u16 length;    /**< Length of the payload */
} GameTLVHeader_St;

/**
 * @struct GameSyncPayload
 * @brief Payload for synchronizing game state between server and clients.
 */
typedef struct {
    int current_player;     /**< Index of the current player */
    int active_color;       /**< Current active color */
    Card top_card;          /**< Card on top of the discard pile */
    int hand_sizes[4];      /**< Number of cards in each player's hand */
    int status;             /**< Game status (0: WAITING, 1: PLAYING) */
    int host_id;            /**< ID of the host player */
} GameSyncPayload;

typedef struct {
    int card_index;
    int chosen_color;
} ActionPlayPayload_St;
#pragma pack(pop)

/**
 * @struct KingServerState
 * @brief Internal state maintained by the server for a game instance.
 */
typedef struct {
    GameState state;    /**< Core game logic state */
    int status;         /**< 0 = WAITING, 1 = PLAYING */
    float bot_timer;    /**< Timer for bot actions */
    BroadcastMessage_Ft broadcast; /**< Last used broadcast function */
} KingServerState;

/**
 * @brief Initializes a game instance on the server.
 * @return A pointer to the newly created KingServerState.
 */
void* king_create_instance(void) {
    KingServerState* ks = calloc(1, sizeof(KingServerState));
    if (ks) {
        init_game_logic(&ks->state);
        init_uno_deck(&ks->state.draw_pile);
        shuffle_deck(&ks->state.draw_pile);
        ks->status = 0; // WAITING
        ks->bot_timer = 0;
        ks->broadcast = NULL;
    }
    return ks;
}

static void broadcast_sync(KingServerState* ks, BroadcastMessage_Ft broadcast) {
    if (!broadcast) return;
    GameState* g = &ks->state;
    Card top_card = {CARD_BLACK, ZERO};
    if (g->discard_pile.head != NULL) {
        top_card = g->discard_pile.head->card;
    }

    GameSyncPayload sync;
    sync.current_player = g->current_player;
    sync.active_color = g->active_color;
    sync.top_card = top_card;
    sync.status = ks->status;
    sync.host_id = (g->num_players > 0) ? g->players[0].id : -1;
    for (int i = 0; i < 4; i++) {
        sync.hand_sizes[i] = (i < g->num_players) ? g->players[i].hand.size : 0;
    }

    u8 buf[2048];
    GameTLVHeader_St tlv_sync = { .game_id = 1, .action = ACTION_SYNC_GAME, .length = sizeof(GameSyncPayload) };
    memcpy(buf, &tlv_sync, sizeof(tlv_sync));
    memcpy(buf + sizeof(tlv_sync), &sync, sizeof(sync));
    broadcast(0, -1, 5, buf, sizeof(tlv_sync) + sizeof(sync));
    
    for (int i = 0; i < g->num_players; i++) {
        if (g->players[i].id < 0) continue; // Skip bots for hand sync
        int target_player_id = g->players[i].id;
        int hand_count = g->players[i].hand.size;
        
        Card* cards = malloc(hand_count * sizeof(Card));
        Node* curr = g->players[i].hand.head;
        for (int j = 0; j < hand_count; j++) {
            cards[j] = curr->card;
            curr = curr->next;
        }
        
        GameTLVHeader_St tlv_hand = { .game_id = 1, .action = ACTION_SYNC_HAND, .length = hand_count * sizeof(Card) };
        memcpy(buf, &tlv_hand, sizeof(tlv_hand));
        memcpy(buf + sizeof(tlv_hand), cards, hand_count * sizeof(Card));
        
        broadcast(-1, target_player_id, 5, buf, sizeof(tlv_hand) + hand_count * sizeof(Card));
        free(cards);
    }
}

/**
 * @brief Processes client actions and broadcasts updates.
 */
void king_on_action(void *state, int player_id, u8 action, void *payload, u16 len, BroadcastMessage_Ft broadcast) {
    if (action != 5 /* ACTION_GAME_DATA */) return;

    if (len < sizeof(GameTLVHeader_St)) return;
    GameTLVHeader_St* tlv = (GameTLVHeader_St*)payload;
    if (tlv->game_id != 1) return; 
    
    u8 real_action = tlv->action;
    void* real_payload = (u8*)payload + sizeof(GameTLVHeader_St);

    KingServerState* ks = (KingServerState*)state;
    ks->broadcast = broadcast; // Store it
    GameState* g = &ks->state;

    int internal_id = -1;
    for (int i = 0; i < g->num_players; i++) {
        if (g->players[i].id == player_id) {
            internal_id = i;
            break;
        }
    }

    if (real_action == ACTION_JOIN_GAME && internal_id == -1) {
        if (g->num_players < 4 && ks->status == 0) {
            internal_id = g->num_players++;
            init_player(&g->players[internal_id], player_id, "Joueur");
            printf("[KING] Nouveau joueur enregistré: %d (Slot %d)\n", player_id, internal_id);
            
            u8 buf_ack[1024];
            GameTLVHeader_St tlv_ack = { .game_id = 1, .action = ACTION_JOIN_ACK, .length = sizeof(int) };
            memcpy(buf_ack, &tlv_ack, sizeof(tlv_ack));
            memcpy(buf_ack + sizeof(tlv_ack), &internal_id, sizeof(int));
            broadcast(-1, player_id, 5, buf_ack, sizeof(tlv_ack) + sizeof(int));
        }
    }

    if (internal_id != -1) {
        if (real_action == ACTION_START_GAME && internal_id == 0 && ks->status == 0) {
            int total_requested = 4;
            if (tlv->length >= sizeof(int)) memcpy(&total_requested, real_payload, sizeof(int));
            if (total_requested < g->num_players) total_requested = g->num_players;
            if (total_requested > 4) total_requested = 4;

            while (g->num_players < total_requested) {
                int bot_idx = g->num_players++;
                init_player(&g->players[bot_idx], -(bot_idx + 1), "Bot");
                printf("[KING] Bot ajouté (Slot %d)\n", bot_idx);
            }

            distribute_cards(g);
            ks->status = 1; 
            printf("[KING] Partie démarrée avec %d joueurs.\n", g->num_players);
        }
        else if (real_action == ACTION_PLAY_CARD && ks->status == 1 && internal_id == g->current_player) {
            ActionPlayPayload_St p;
            memcpy(&p, real_payload, sizeof(ActionPlayPayload_St));
            
            // On récupère la carte avant de jouer pour savoir si c'est un joker
            Node* curr = g->players[internal_id].hand.head;
            for(int i=0; i < p.card_index && curr; i++) curr = curr->next;
            
            if (curr && try_play_card(g, internal_id, p.card_index)) {
                Card played = g->discard_pile.head->card;

                // Si c'était un joker, on applique la couleur choisie
                if (played.color == CARD_BLACK) {
                    g->active_color = p.chosen_color;
                }
                
                // Logique simplifiée des effets :
                int skip = 0;
                if (played.value == SKIP || played.value == PLUS_TWO || played.value == PLUS_FOUR) skip = 1;
                
                if (played.value == REVERSE) {
                    if (g->num_players == 2) skip = 1;
                    else g->game_direction *= -1;
                }

                int step = g->game_direction * (1 + skip);
                g->current_player = (g->current_player + step + g->num_players) % g->num_players;
                
                // Effets de pioche
                if (played.value == PLUS_TWO) {
                    for(int i=0; i<2; i++) player_draw_card(g, g->current_player);
                } else if (played.value == PLUS_FOUR) {
                    for(int i=0; i<4; i++) player_draw_card(g, g->current_player);
                }
            }
        } else if (real_action == ACTION_DRAW_CARD && ks->status == 1 && internal_id == g->current_player) {
            player_draw_card(g, internal_id);
            g->current_player = (g->current_player + g->game_direction + g->num_players) % g->num_players;
        } else if (real_action == ACTION_QUIT_GAME) {
            printf("[KING] Joueur %d quitte la room.\n", player_id);
        }
    }

    broadcast_sync(ks, broadcast);
}

void king_on_tick(void* state) {
    KingServerState* ks = (KingServerState*)state;
    if (ks->status != 1 || !ks->broadcast) return;

    GameState* g = &ks->state;
    int cp = g->current_player;

    if (g->players[cp].id < 0) { // C'est un bot
        ks->bot_timer += 0.016f; // Simulated delta
        if (ks->bot_timer > 1.0f) { // 1 seconde de réflexion
            ks->bot_timer = 0;
            int card_idx = -1;
            calculate_best_move(g, cp, &card_idx);
            
            if (card_idx != -1) {
                Node* curr = g->players[cp].hand.head;
                for(int i=0; i<card_idx; i++) curr = curr->next;
                
                Card toPlay = curr->card;
                if (try_play_card(g, cp, card_idx)) {
                    if (toPlay.color == CARD_BLACK) {
                        g->active_color = (int)(rand() % 4); // Bot chooses random color
                    }

                    int skip = 0;
                    if (toPlay.value == SKIP || toPlay.value == PLUS_TWO || toPlay.value == PLUS_FOUR) skip = 1;
                    if (toPlay.value == REVERSE) {
                        if (g->num_players == 2) skip = 1;
                        else g->game_direction *= -1;
                    }

                    int step = g->game_direction * (1 + skip);
                    g->current_player = (g->current_player + step + g->num_players) % g->num_players;

                    if (toPlay.value == PLUS_TWO) {
                        for(int i=0; i<2; i++) player_draw_card(g, g->current_player);
                    } else if (toPlay.value == PLUS_FOUR) {
                        for(int i=0; i<4; i++) player_draw_card(g, g->current_player);
                    }
                }
            } else {
                player_draw_card(g, cp);
                g->current_player = (g->current_player + g->game_direction + g->num_players) % g->num_players;
            }
            broadcast_sync(ks, ks->broadcast);
        }
    }
}

void king_on_player_leave(void* state, int player_id) {
    KingServerState* ks = (KingServerState*)state;
    GameState* g = &ks->state;
    
    int internal_id = -1;
    for (int i = 0; i < g->num_players; i++) {
        if (g->players[i].id == player_id) {
            internal_id = i;
            break;
        }
    }

    if (internal_id != -1) {
        printf("[KING] Joueur %d a quitté.\n", player_id);
        // Transform into bot if game is running
        if (ks->status == 1) {
            g->players[internal_id].id = -(internal_id + 1);
            strcpy(g->players[internal_id].name, "Bot (ex-humain)");
        } else {
            // Remove player if in waiting
            for (int i = internal_id; i < g->num_players - 1; i++) {
                g->players[i] = g->players[i+1];
            }
            g->num_players--;
        }
        if (ks->broadcast) broadcast_sync(ks, ks->broadcast);
    }
}

void king_destroy_instance(void *state) {
    KingServerState* ks = (KingServerState*)state;
    for (int i = 0; i < 4; i++) {
        clear_deck(&ks->state.players[i].hand);
    }
    clear_deck(&ks->state.draw_pile);
    clear_deck(&ks->state.discard_pile);
    free(ks);
}

GameServerInterface_St king_module = {
    .game_name = "king-for-four",
    .create_instance = king_create_instance,
    .on_action = king_on_action,
    .on_tick = king_on_tick, 
    .on_player_leave = king_on_player_leave,
    .destroy_instance = king_destroy_instance
};
