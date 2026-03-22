/**
 * @file game.c
 * @author i-Charlys (CAILLON Charles)
 * @date 2026-03-18
 * @brief Implementation of the main game logic for King for Four.
 */

#include "core/game.h"
#include "core/card.h"
#include "baseTypes.h"
#include <stddef.h>
#include <stdio.h>

/**
 * @brief Rule matrix for valid color moves.
 * [Discard Pile Color (0-3)][Played Card Color (0-4)]
 * 1 indicates a valid move, 0 indicates an invalid move.
 */
s32 valid_color[4][5] = {
    {1, 0, 0, 0, 1},
    {0, 1, 0, 0, 1},
    {0, 0, 1, 0, 1},
    {0, 0, 0, 1, 1}
};

/**
 * @brief Rule matrix for valid value moves.
 * [Discard Pile Value (0-14)][Played Card Value (0-14)]
 */
s32 valid_value[15][15] = {0};

/**
 * @brief Initializes the game logic, rules, and state.
 * @param g Pointer to the GameState to initialize.
 */
void init_game_logic(GameState* g) {
    for (s32 i = 0; i < 15; i++) valid_value[i][i] = 1;

    if (!g) return;
    g->current_player = 0;
    g->game_direction = 1;
    g->active_color = -1;
    g->num_players = 0;
    g->draw_pile = (Deck){NULL, 0};
    g->discard_pile = (Deck){NULL, 0};
    
    // INITIALISATION CRITIQUE DES MAINS
    for (s32 i = 0; i < 4; i++) {
        g->players[i].hand.head = NULL;
        g->players[i].hand.size = 0;
    }
}

/**
 * @brief Checks if a move is valid based on color or value matching.
 * @param active_color The current active color (set by special cards).
 * @param played The card being played.
 * @param top The card on top of the discard pile.
 * @return 1 if the move is valid, 0 otherwise.
 */
s32 is_move_valid(s32 active_color, Card played, Card top) {
    s32 idx = (active_color >= 0 && active_color < 4) ? active_color : (s32)top.color;
    
    s32 color_ok = (idx >= 0 && idx < 4) ? valid_color[idx][played.color] : 0;
    s32 value_ok = valid_value[top.value][played.value];

    return color_ok || value_ok;
}

/**
 * @brief Distributes 7 cards to each player and starts the discard pile.
 * @param g Pointer to the GameState.
 */
void distribute_cards(GameState* g) {
    if (!g || g->num_players <= 0) return;

    for (s32 i = 0; i < g->num_players; i++) {
        for (s32 j = 0; j < 7; j++) draw_to_hand(&(g->players[i]), &(g->draw_pile));
    }

    Card first = pop_card(&(g->draw_pile));
    push_card(&(g->discard_pile), first);
    
    /* Couleur forcée si la première carte est un Joker */
    g->active_color = (first.color == 4) ? 0 : (s32)first.color;
}

/**
 * @brief Processes a player's attempt to play a card.
 * @param g Pointer to the GameState.
 * @param playerIndex Index of the player.
 * @param cardIndex Index of the card in the player's hand.
 * @return 1 on success, 0 on failure.
 */
s32 try_play_card(GameState *g, s32 playerIndex, s32 cardIndex) {
    Player* p = &g->players[playerIndex];
    
    // 1. Trouver la carte dans la main sans la retirer 
    Node* current = p->hand.head;
    for (s32 i = 0; i < cardIndex; i++) {
        if (current) current = current->next;
    }
    if (!current) return 0; // Erreur d'index

    Card cardToPlay = current->card;
    
    // 2. Récupérer la carte du dessus du talon
    if (g->discard_pile.head == NULL) return 0; // Erreur technique
    Card topCard = g->discard_pile.head->card;

    // 3. Vérifier les règles
    if (is_move_valid(g->active_color, cardToPlay, topCard)) {
        
        // --- LE COUP EST VALIDE ---
        
        // A. On retire la carte de la main
        Card played = remove_at(&p->hand, cardIndex);
        
        // B. On la met sur le talon
        push_card(&g->discard_pile, played);
        
        // C. On met à jour la couleur active
        if (played.color == CARD_BLACK) {
            // TODO: Ouvrir un menu pour choisir la couleur
            g->active_color = CARD_RED; // Temporaire pour tester
        } else {
            g->active_color = -1;
        }
        
        printf("Carte jouée ! Reste : %d cartes\n", p->hand.size);
        return 1; // Succès
    }
    
    printf("Coup invalide !\n");
    return 0; // Échec
}

/**
 * @brief Handles drawing a card for a player, including recycling the discard pile if needed.
 * @param g Pointer to the GameState.
 * @param playerIndex Index of the player drawing.
 * @return 1 on success, 0 if no cards are available.
 */
s32 player_draw_card(GameState *g, s32 playerIndex) {
    Player* p = &g->players[playerIndex];

    // 1. Si la pioche est vide, on recycle le talon
    if (g->draw_pile.size == 0) {
        if (g->discard_pile.size <= 1) {
            printf("Plus aucune carte nulle part !\n");
            return 0; // Vraiment plus de cartes
        }

        printf("Pioche vide ! On mélange le talon...\n");
        
        // On garde la carte du dessus du talon
        Card topVisible = pop_card(&g->discard_pile);
        
        // On déplace tout le reste du talon vers la pioche
        while(g->discard_pile.head != NULL) {
            push_card(&g->draw_pile, pop_card(&g->discard_pile));
        }
        
        // On remet la carte visible sur le talon
        push_card(&g->discard_pile, topVisible);
        
        // On mélange la nouvelle pioche
        shuffle_deck(&g->draw_pile);
    }

    // 2. On pioche
    draw_to_hand(p, &g->draw_pile);
    return 1;
}
