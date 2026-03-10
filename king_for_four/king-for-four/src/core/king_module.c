/**
 * @file king_module.c
 * @brief Implémentation réseau du module King-for-Four.
 * Ce module enveloppe les règles métier (game.h) dans l'interface réseau standard (game_interface.h).
 * @author i-Charlys (CAILLON Charles) - Adaptation Serveur Autoritaire
 * @date 2026-03-11
 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/* Inclusion des entêtes fournis */
#include "core/game.h"
#include "core/card.h"
#include "core/player.h"
#include "../../Multi-Mini-games/firstparty/game_interface.h"

/* Définition des actions réseau strictes (Payloads binaires) */
#define KING_ACTION_JOIN       10
#define KING_ACTION_PLAY_CARD  11
#define KING_ACTION_DRAW_CARD  12
#define KING_ACTION_SYNC       13

/**
 * @struct PlayCardPayload
 * @brief Format binaire attendu lorsqu'un client veut jouer une carte.
 */
#pragma pack(push, 1)
typedef struct {
    int card_index;
    int requested_color; // Pour le joker
} PlayCardPayload;
#pragma pack(pop)

/**
 * @brief Alloue et initialise une nouvelle instance de jeu côté serveur.
 * @return void* Pointeur vers le GameState initialisé.
 */
void* king_create() {
    GameState* state = (GameState*)malloc(sizeof(GameState));
    if (state != NULL) {
        memset(state, 0, sizeof(GameState));
        init_game_logic(state);
        init_uno_deck(&(state->draw_pile));
        shuffle_deck(&(state->draw_pile));
        
        // La distribution se fera quand les joueurs rejoignent
        printf("[KING_MODULE] Nouvelle instance allouée et pioche mélangée.\n");
    }
    return state;
}

/**
 * @brief Diffuse l'état complet à tous les clients pour la synchronisation (Anti-désync).
 * Fonction interne utilitaire.
 */
static void broadcast_state_sync(GameState *state, broadcast_func_t broadcast) {
    // Dans un cas réel de production optimisé, on n'envoie que les deltas.
    // Ici, on broadcast l'ID du joueur courant et la carte du talon.
    uint8_t sync_buffer[256];
    memset(sync_buffer, 0, sizeof(sync_buffer));
    
    int offset = 0;
    memcpy(sync_buffer + offset, &(state->current_player), sizeof(int));
    offset += sizeof(int);
    memcpy(sync_buffer + offset, &(state->active_color), sizeof(int));
    offset += sizeof(int);
    
    if (state->discard_pile.head != NULL) {
        memcpy(sync_buffer + offset, &(state->discard_pile.head->card), sizeof(Card));
        offset += sizeof(Card);
    }

    // Le room_id est fixé à 0, exclude_id à -1 (personne n'est exclu)
    broadcast(0, -1, KING_ACTION_SYNC, sync_buffer, (uint16_t)offset);
}

/**
 * @brief Traite les requêtes entrantes des clients de façon sécurisée (Serveur Autoritaire).
 */
void king_on_action(void *state, int player_id, uint8_t action, void *payload, uint16_t len, broadcast_func_t broadcast) {
    GameState *g = (GameState*)state;

    if (g == NULL) return;

    switch (action) {
        case KING_ACTION_JOIN: {
            if (g->num_players < 4) {
                // Initialise le nouveau joueur
                char default_name[50];
                snprintf(default_name, 50, "Joueur %d", player_id);
                init_player(&(g->players[g->num_players]), player_id, default_name);
                g->num_players++;
                
                // Si la table est pleine (ex: 4 joueurs), on lance la distribution
                if (g->num_players == 4) { // Fourchette [2-4] adaptable
                    distribute_cards(g);
                    broadcast_state_sync(g, broadcast);
                    printf("[KING_MODULE] La partie commence !\n");
                }
            }
            break;
        }

        case KING_ACTION_PLAY_CARD: {
            // Vérification stricte : est-ce bien le tour de ce joueur ?
            if (g->players[g->current_player].id != player_id) {
                printf("[KING_MODULE] Refus : Ce n'est pas le tour du joueur %d.\n", player_id);
                return; 
            }

            if (len >= sizeof(PlayCardPayload)) {
                PlayCardPayload *p_card = (PlayCardPayload*)payload;
                
                // La fonction try_play_card (définie dans game.c) gère la validation des règles.
                if (try_play_card(g, g->current_player, p_card->card_index) == 1) {
                    // Si Joker joué, forcer la couleur
                    if (g->discard_pile.head != NULL && g->discard_pile.head->card.color == CARD_BLACK) {
                        g->active_color = p_card->requested_color;
                    }

                    // Avancement du tour (horaire / anti-horaire)
                    g->current_player = (g->current_player + g->game_direction + g->num_players) % g->num_players;
                    
                    // Diffusion du nouvel état validé par le serveur
                    broadcast_state_sync(g, broadcast);
                }
            }
            break;
        }

        case KING_ACTION_DRAW_CARD: {
            if (g->players[g->current_player].id != player_id) return;

            if (player_draw_card(g, g->current_player) == 1) {
                // Le joueur passe son tour après avoir pioché
                g->current_player = (g->current_player + g->game_direction + g->num_players) % g->num_players;
                broadcast_state_sync(g, broadcast);
            }
            break;
        }

        default:
            printf("[KING_MODULE] Action %d non reconnue.\n", action);
            break;
    }
}

/**
 * @brief Gère la déconnexion inattendue d'un joueur.
 */
void king_on_player_leave(void *state, int player_id) {
    GameState *g = (GameState*)state;
    if (g == NULL) return;

    printf("[KING_MODULE] Joueur %d a quitté la partie.\n", player_id);
    
    // Remise des cartes du joueur dans la pioche pour ne pas perdre l'intégrité du deck
    for (int i = 0; i < g->num_players; i++) {
        if (g->players[i].id == player_id) {
            while (g->players[i].hand.size > 0) {
                Card c = pop_card(&(g->players[i].hand));
                push_card(&(g->draw_pile), c);
            }
            // Shuffle pour anonymiser les cartes récupérées
            shuffle_deck(&(g->draw_pile));
            break;
        }
    }
}

/**
 * @brief Tick régulier. Utilisable pour valider des timers de tour (ex: 30s par joueur).
 */
void king_tick(void *state) {
    // Réservé pour implémenter un compteur de temps par tour (Anti-AFK).
    (void)state;
}

/**
 * @brief Destruction et nettoyage de la mémoire vive.
 */
void king_destroy(void *state) {
    if (state != NULL) {
        GameState *g = (GameState*)state;
        free_deck(&(g->draw_pile));
        free_deck(&(g->discard_pile));
        for(int i = 0; i < g->num_players; i++) {
            free_deck(&(g->players[i].hand));
        }
        free(g);
        printf("[KING_MODULE] Instance détruite avec succès.\n");
    }
}

/**
 * @brief Exportation de l'interface réseau pour le Linker.
 * Remplacera 'lobby_module' dans server.c.
 */
GameInterface king_module = {
    .game_name = "king-for-four",
    .create_instance = king_create,
    .on_action = king_on_action,
    .on_tick = king_tick,
    .on_player_leave = king_on_player_leave,
    .destroy_instance = king_destroy
};