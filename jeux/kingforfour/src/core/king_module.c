/**
 * @file king_module.c
 * @brief Module serveur pour le jeu King-for-Four (Uno).
 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "core/game.h"
#include "core/card.h"
#include "core/player.h"
#include "game_interface.h"

#define ACTION_PLAY_CARD 0x10
#define ACTION_DRAW_CARD 0x11
#define ACTION_SYNC_GAME 0x12
#define ACTION_JOIN_GAME 0x13
#define ACTION_START_GAME 0x14
#define ACTION_SYNC_HAND 0x15
#define ACTION_JOIN_ACK  0x16

#pragma pack(push, 1)
typedef struct {
    uint8_t game_id;
    uint8_t action;
    uint16_t length;
} GameTLVHeader;

typedef struct {
    int current_player;
    int active_color;
    Card top_card;
    int hand_sizes[4];
    int status; // 0: WAITING, 1: PLAYING
    int host_id;
} GameSyncPayload;
#pragma pack(pop)

typedef struct {
    GameState state;
    int status; // 0 = WAITING, 1 = PLAYING
} KingServerState;

/** @brief Initialise une partie sur le serveur. */
void* king_create_instance() {
    KingServerState* ks = calloc(1, sizeof(KingServerState));
    if (ks) {
        init_game_logic(&ks->state);
        init_uno_deck(&ks->state.draw_pile);
        shuffle_deck(&ks->state.draw_pile);
        ks->status = 0; // WAITING
    }
    return ks;
}

/** @brief Traite les clics/actions des clients. */
void king_on_action(void *state, int player_id, uint8_t action, void *payload, uint16_t len, broadcast_func_t broadcast) {
    if (action != 5 /* ACTION_GAME_DATA */) return;

    if (len < sizeof(GameTLVHeader)) return;
    GameTLVHeader* tlv = (GameTLVHeader*)payload;
    if (tlv->game_id != 1) return; // Pas pour King For Four
    
    uint8_t real_action = tlv->action;
    void* real_payload = (uint8_t*)payload + sizeof(GameTLVHeader);

    KingServerState* ks = (KingServerState*)state;
    GameState* g = &ks->state;

    // 1. GESTION DES CONNEXIONS
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
            
            // Unicast ACK au joueur pour lui donner son internal_id
            uint8_t buf_ack[1024];
            GameTLVHeader tlv_ack = { .game_id = 1, .action = ACTION_JOIN_ACK, .length = sizeof(int) };
            memcpy(buf_ack, &tlv_ack, sizeof(tlv_ack));
            memcpy(buf_ack + sizeof(tlv_ack), &internal_id, sizeof(int));
            broadcast(-1, player_id, 5, buf_ack, sizeof(tlv_ack) + sizeof(int));
        }
    }

    // 2. ACTIONS RÉSEAU (Seulement si le joueur est dans la room)
    if (internal_id != -1) {
        if (real_action == ACTION_START_GAME && internal_id == 0 && ks->status == 0) {
            distribute_cards(g);
            ks->status = 1; // PLAYING
            printf("[KING] Partie démarrée par l'hôte (Joueur %d).\n", player_id);
        }
        else if (real_action == ACTION_PLAY_CARD && ks->status == 1 && internal_id == g->current_player) {
            int card_index = *((int*)real_payload);
            if (try_play_card(g, internal_id, card_index)) {
                g->current_player = (g->current_player + g->game_direction + g->num_players) % g->num_players;
            }
        } else if (real_action == ACTION_DRAW_CARD && ks->status == 1 && internal_id == g->current_player) {
            player_draw_card(g, internal_id);
            g->current_player = (g->current_player + g->game_direction + g->num_players) % g->num_players;
        }
    }

    // 3. SYNCHRONISATION
    /*
     +-----------------------------------------------------------------+
     | SYNC GLOBALE (Broadcast) : Talon, Couleur, Qui joue, Nb cartes  |
     +-----------------------------------------------------------------+
     | SYNC PRIVÉE  (Unicast)   : Liste des cartes de MA main          |
     +-----------------------------------------------------------------+
    */
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

    uint8_t buf[2048];
    // A. Broadcast de l'état global
    GameTLVHeader tlv_sync = { .game_id = 1, .action = ACTION_SYNC_GAME, .length = sizeof(GameSyncPayload) };
    memcpy(buf, &tlv_sync, sizeof(tlv_sync));
    memcpy(buf + sizeof(tlv_sync), &sync, sizeof(sync));
    broadcast(0, -1, 5, buf, sizeof(tlv_sync) + sizeof(sync));
    
    // B. Unicast des mains
    for (int i = 0; i < g->num_players; i++) {
        int target_player_id = g->players[i].id;
        int hand_count = g->players[i].hand.size;
        
        Card* cards = malloc(hand_count * sizeof(Card));
        Node* curr = g->players[i].hand.head;
        for (int j = 0; j < hand_count; j++) {
            cards[j] = curr->card;
            curr = curr->next;
        }
        
        GameTLVHeader tlv_hand = { .game_id = 1, .action = ACTION_SYNC_HAND, .length = hand_count * sizeof(Card) };
        memcpy(buf, &tlv_hand, sizeof(tlv_hand));
        memcpy(buf + sizeof(tlv_hand), cards, hand_count * sizeof(Card));
        
        broadcast(-1, target_player_id, 5, buf, sizeof(tlv_hand) + hand_count * sizeof(Card));
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
