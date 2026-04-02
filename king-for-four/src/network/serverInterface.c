/**
 * @file king_module.c
 * @author i-Charlys
 * @date 2026-03-18
 * @brief Server-side module for the King-for-Four (Uno) game.
 */

#include "APIs/generalAPI.h"
#include "core/game.h"
#include "core/card.h"
#include "core/player.h"
#include "core/bot.h"

#include "networkInterface.h"
#include "rand.h"
#include "APIs/generalAPI.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum {
    ACTION_CODE_KFF_PLAY_CARD = firstAvailableActionCode,
    ACTION_CODE_KFF_DRAW_CARD,
    ACTION_CODE_KFF_SYNC_HAND,
};

#pragma pack(push, 1)
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
    int last_player_id;     /**< ID of the last player who moved */
    int last_action;        /**< 0: Play, 1: Draw */
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
    float bot_target_time; /**< Randomized thinking time */
    int last_player_id;
    int last_action;
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
        ks->bot_target_time = 1.0f + prng_randf() * 2.0f; 
        ks->last_player_id = -1;
        ks->last_action = -1;
        ks->broadcast = NULL;
    }
    return ks;
}

static void broadcast_sync(KingServerState* ks, BroadcastMessage_Ft broadcast) {
    if (!broadcast) return;
    GameState* g = &ks->state;
    Card top_card = {CARD_BLACK, ZERO};
    if (g->discard_pile.size > 0) {
        top_card = g->discard_pile.cards[g->discard_pile.size - 1];
    }

    GameSyncPayload sync;
    sync.current_player = g->current_player;
    sync.active_color = g->active_color;
    sync.top_card = top_card;
    sync.status = ks->status;
    sync.host_id = (g->num_players > 0) ? g->players[0].id : -1;
    sync.last_player_id = ks->last_player_id;
    sync.last_action = ks->last_action;
    for (int i = 0; i < 4; i++) {
        sync.hand_sizes[i] = (i < g->num_players) ? g->players[i].hand.size : 0;
    }

    u8 buf[2048];
    GameTLVHeader_St tlv_sync = { .game_id = MINI_GAME_KFF, .action = ACTION_CODE_SYNC_GAME, .length = sizeof(GameSyncPayload) };
    memcpy(buf, &tlv_sync, sizeof(tlv_sync));
    memcpy(buf + sizeof(tlv_sync), &sync, sizeof(sync));
    broadcast(0, -1, ACTION_GAME_DATA, buf, (u16)(sizeof(tlv_sync) + sizeof(sync)));
    
    for (int i = 0; i < g->num_players; i++) {
        if (g->players[i].id < 0) continue; // Skip bots for hand sync
        int target_player_id = g->players[i].id;
        int hand_count = g->players[i].hand.size;
        
        Card* cards = malloc((size_t)hand_count * sizeof(Card));
        for (int j = 0; j < hand_count; j++) {
            cards[j] = g->players[i].hand.cards[j];
        }
        
<<<<<<< HEAD
        GameTLVHeader_St tlv_hand = { .game_id = MINI_GAME_KFF, .action = ACTION_CODE_KFF_SYNC_HAND, .length = (u16)(hand_count * sizeof(Card)) };
=======
        GameTLVHeader_St tlv_hand = { .game_id = MINI_GAME_KFF, .action = ACTION_CODE_KFF_SYNC_HAND, .length = hand_count * sizeof(Card) };
>>>>>>> origin/mgit-PR1-20-03
        memcpy(buf, &tlv_hand, sizeof(tlv_hand));
        memcpy(buf + sizeof(tlv_hand), cards, (size_t)hand_count * sizeof(Card));
        
<<<<<<< HEAD
        broadcast(-(target_player_id + 1), 999, ACTION_GAME_DATA, buf, (u16)(sizeof(tlv_hand) + hand_count * sizeof(Card)));
=======
        broadcast(UNICAST, target_player_id, 5, buf, sizeof(tlv_hand) + hand_count * sizeof(Card));
>>>>>>> origin/mgit-PR1-20-03
        free(cards);
    }
}

/**
 * @brief Processes client actions and broadcasts updates.
 */
void king_on_action(void *state, int player_id, u8 action, const void *payload, u16 len, BroadcastMessage_Ft broadcast) {
<<<<<<< HEAD
    if (action != ACTION_GAME_DATA) return;
=======
    if (action != ACTION_CODE_GAME_DATA) return;
>>>>>>> origin/mgit-PR1-20-03

    if (len < sizeof(GameTLVHeader_St)) return;
    GameTLVHeader_St* tlv = (GameTLVHeader_St*)payload;
    if (tlv->game_id != MINI_GAME_KFF) return; 
    
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

<<<<<<< HEAD
    // Si le joueur n'est pas dans la liste mais envoie une action, on essaie de le JOIN automatiquement
    // (Utile si le serveur a redémarré ou si le joueur a été déco/reco silencieusement)
    if (internal_id == -1 && ks->status == 0 && g->num_players < 4) {
        internal_id = g->num_players++;
        init_player(&g->players[internal_id], player_id, "Joueur");
        printf("[KING] Auto-Join du joueur: %d (Slot %d)\n", player_id, internal_id);
        
        // On envoie quand même l'ACK pour que le client sache son ID
        u8 buf_ack[1024];
        GameTLVHeader_St tlv_ack = { .game_id = MINI_GAME_KFF, .action = ACTION_CODE_JOIN_ACK, .length = sizeof(int) };
        memcpy(buf_ack, &tlv_ack, sizeof(tlv_ack));
        memcpy(buf_ack + sizeof(tlv_ack), &internal_id, sizeof(int));
        broadcast(-(player_id + 1), 999, ACTION_GAME_DATA, buf_ack, sizeof(tlv_ack) + sizeof(int));
    }

    printf("[KING] Action reçue: 0x%02X de P%d (internal_id: %d)\n", real_action, player_id, internal_id);

    if (real_action == ACTION_CODE_JOIN_GAME && internal_id != -1) {
        // Déjà géré par l'auto-join ou existant, on renvoie juste l'ACK si demandé explicitement
        u8 buf_ack[1024];
        GameTLVHeader_St tlv_ack = { .game_id = MINI_GAME_KFF, .action = ACTION_CODE_JOIN_ACK, .length = sizeof(int) };
        memcpy(buf_ack, &tlv_ack, sizeof(tlv_ack));
        memcpy(buf_ack + sizeof(tlv_ack), &internal_id, sizeof(int));
        broadcast(-(player_id + 1), 999, ACTION_GAME_DATA, buf_ack, sizeof(tlv_ack) + sizeof(int));
=======
    if (real_action == ACTION_CODE_JOIN_GAME && internal_id == -1) {
        if (g->num_players < 4 && ks->status == 0) {
            internal_id = g->num_players++;
            init_player(&g->players[internal_id], player_id, "Joueur");
            printf("[KING] Nouveau joueur enregistré: %d (Slot %d)\n", player_id, internal_id);
            
            u8 buf_ack[1024];
            GameTLVHeader_St tlv_ack = { .game_id = MINI_GAME_KFF, .action = ACTION_CODE_JOIN_ACK, .length = sizeof(int) };
            memcpy(buf_ack, &tlv_ack, sizeof(tlv_ack));
            memcpy(buf_ack + sizeof(tlv_ack), &internal_id, sizeof(int));
            broadcast(UNICAST, player_id, ACTION_CODE_GAME_DATA, buf_ack, sizeof(tlv_ack) + sizeof(int));
        }
>>>>>>> origin/mgit-PR1-20-03
    }

    if (internal_id != -1) {
        if (real_action == ACTION_CODE_START_GAME && internal_id == 0 && ks->status == 0) {
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
            
            // On force une synchro immédiate pour tout le monde
            broadcast_sync(ks, broadcast);
            return; // On évite le broadcast_sync final en double
        }
        else if (real_action == ACTION_CODE_KFF_PLAY_CARD && ks->status == 1 && internal_id == g->current_player) {
            ActionPlayPayload_St p;
            memcpy(&p, real_payload, sizeof(ActionPlayPayload_St));
            
            // On récupère la carte avant de jouer pour savoir si c'est un joker
            if (p.card_index >= 0 && p.card_index < g->players[internal_id].hand.size && try_play_card(g, internal_id, p.card_index)) {
                Card played = g->discard_pile.cards[g->discard_pile.size - 1];
                
                ks->last_player_id = internal_id;
                ks->last_action = 0; // Play

                // Si c'était un joker, on applique la couleur choisie
                if (played.color == CARD_BLACK) {
                    g->active_color = p.chosen_color;
                }
                
                // Logique simplifiée des effets :
                int next_p = (g->current_player + g->game_direction + g->num_players) % g->num_players;
                int skip = 0;
                
                if (played.value == PLUS_TWO) {
                    for(int i=0; i<2; i++) player_draw_card(g, next_p);
                    skip = 1;
                } else if (played.value == PLUS_FOUR) {
                    for(int i=0; i<4; i++) player_draw_card(g, next_p);
                    skip = 1;
                } else if (played.value == SKIP) {
                    skip = 1;
                } else if (played.value == REVERSE) {
                    if (g->num_players == 2) skip = 1;
                    else g->game_direction *= -1;
                }

                int step = g->game_direction * (1 + skip);
                g->current_player = (g->current_player + step + g->num_players) % g->num_players;
            } else {
                if (p.card_index >= 0 && p.card_index < g->players[internal_id].hand.size) {
                    Card attempted = g->players[internal_id].hand.cards[p.card_index];
                    Card top = g->discard_pile.cards[g->discard_pile.size - 1];
                    printf("[KING SERVER] Coup invalide de P%d: [%d %d] sur [%d %d] (active_color: %d)\n", 
                           internal_id, attempted.color, attempted.value, top.color, top.value, g->active_color);
                }
            }
        } else if (real_action == ACTION_CODE_KFF_DRAW_CARD && ks->status == 1 && internal_id == g->current_player) {
            player_draw_card(g, internal_id);
            ks->last_player_id = internal_id;
            ks->last_action = 1; // Draw
            g->current_player = (g->current_player + g->game_direction + g->num_players) % g->num_players;
        } else if (real_action == ACTION_CODE_QUIT_GAME) {
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
        if (ks->bot_timer > ks->bot_target_time) { 
            ks->bot_timer = 0;
            ks->bot_target_time = 1.0f + prng_randf() * 1.5f; // 1.0s to 2.5s
            
            int card_idx = -1;
            calculate_best_move(g, cp, &card_idx);
            
            if (card_idx != -1) {
                if (card_idx < g->players[cp].hand.size) {
                    Card toPlay = g->players[cp].hand.cards[card_idx];
                    if (try_play_card(g, cp, card_idx)) {
                        ks->last_player_id = cp;
                        ks->last_action = 0; // Play

                        if (toPlay.color == CARD_BLACK) {
                            g->active_color = (int)(prng_rand() % 4); // Bot chooses random color
                        }

                        int next_p = (g->current_player + g->game_direction + g->num_players) % g->num_players;
                        int skip = 0;
                        
                        if (toPlay.value == PLUS_TWO) {
                            for(int i=0; i<2; i++) player_draw_card(g, next_p);
                            skip = 1;
                        } else if (toPlay.value == PLUS_FOUR) {
                            for(int i=0; i<4; i++) player_draw_card(g, next_p);
                            skip = 1;
                        } else if (toPlay.value == SKIP) {
                            skip = 1;
                        } else if (toPlay.value == REVERSE) {
                            if (g->num_players == 2) skip = 1;
                            else g->game_direction *= -1;
                        }

                        int step = g->game_direction * (1 + skip);
                        g->current_player = (g->current_player + step + g->num_players) % g->num_players;
                    }
                }
            } else {
                player_draw_card(g, cp);
                ks->last_player_id = cp;
                ks->last_action = 1; // Draw
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

GameServerInterface_St kingServerInterface = {
    .game_name          = "king-for-four",
    .create_instance    = king_create_instance,
    .on_action          = king_on_action,
    .on_tick            = king_on_tick, 
    .on_player_leave    = king_on_player_leave,
    .destroy_instance   = king_destroy_instance
};
