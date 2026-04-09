/**
 * @file game.c
 * @brief Core game logic implementation for King for Four.
 */

#include "core/game.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/**
 * @brief Initializes the game logic and state.
 * @param g Pointer to the GameState to initialize.
 */
void init_game_logic(GameState* g) {
    if (!g) return;
    g->current_player = 0;
    g->game_direction = 1;
    g->active_color = -1;
    g->num_players = 0;
    
    clear_deck(&g->draw_pile);
    clear_deck(&g->discard_pile);
    
    for (int i = 0; i < 4; i++) {
        clear_deck(&g->players[i].hand);
    }
}

/**
 * @brief Checks if a move is valid based on color or value matching.
 * @param active_color The current active color (set by special cards).
 * @param played The card being played.
 * @param top The card on top of the discard pile.
 * @return 1 if the move is valid, 0 otherwise.
 */
int is_move_valid(int active_color, Card played, Card top) {
    if (played.color == CARD_BLACK) return 1; // Joker or +4 can always be played

    Card_Color current_match_color = (active_color >= 0 && active_color < 4) ? (Card_Color)active_color : top.color;

    if (played.color == current_match_color) return 1;
    if (played.value == top.value) return 1;

    return 0;
}

/**
 * @brief Distributes initial cards to players and sets up the discard pile.
 * @param g Pointer to the GameState.
 */
void distribute_cards(GameState* g) {
    if (!g || g->num_players == 0) return;

    // 1. Setup draw pile (Full Uno-like deck)
    clear_deck(&g->draw_pile);
    for (int c = 0; c < 4; c++) {
        // Numbers 0-9
        for (int v = ZERO; v <= NINE; v++) {
            push_card(&g->draw_pile, (Card){(Card_Color)c, (Card_Value)v});
            if (v > ZERO) push_card(&g->draw_pile, (Card){(Card_Color)c, (Card_Value)v}); // Two of each 1-9
        }
        // Action cards (two of each)
        push_card(&g->draw_pile, (Card){(Card_Color)c, SKIP});
        push_card(&g->draw_pile, (Card){(Card_Color)c, SKIP});
        push_card(&g->draw_pile, (Card){(Card_Color)c, REVERSE});
        push_card(&g->draw_pile, (Card){(Card_Color)c, REVERSE});
        push_card(&g->draw_pile, (Card){(Card_Color)c, PLUS_TWO});
        push_card(&g->draw_pile, (Card){(Card_Color)c, PLUS_TWO});
    }
    // Wild cards (four of each)
    for (int i = 0; i < 4; i++) {
        push_card(&g->draw_pile, (Card){CARD_BLACK, JOKER});
        push_card(&g->draw_pile, (Card){CARD_BLACK, PLUS_FOUR});
    }

    shuffle_deck(&g->draw_pile);

    // 2. Deal hands
    for (int i = 0; i < g->num_players; i++) {
        for (int j = 0; j < 7; j++) {
            draw_to_hand(&g->players[i], &g->draw_pile);
        }
    }

    // 3. Initial discard
    if (g->draw_pile.size > 0) {
        Card first = pop_card(&g->draw_pile);
        push_card(&g->discard_pile, first);
        
        // Setup initial game state based on first card
        if (first.color == CARD_BLACK) {
            g->active_color = -1; // No color chosen yet, host must pick
        } else {
            g->active_color = (int)first.color;
        }

        // Apply first card effect logic
        int skip = 0;
        if (first.value == SKIP) skip = 1;
        if (first.value == PLUS_TWO) {
            player_draw_card(g, 0); // Next player (index 0 for start) draws 2
            player_draw_card(g, 0);
            skip = 1;
        }
        if (first.value == REVERSE) {
            if (g->num_players == 2) skip = 1;
            else g->game_direction *= -1;
        }
        
        // Adjust starting player
        int offset = skip * g->game_direction;
        g->current_player = (offset % g->num_players + g->num_players) % g->num_players;
    }
}

/**
 * @brief Attempts to play a card from a player's hand.
 * @param g Pointer to the GameState.
 * @param playerIndex Index of the player playing.
 * @param cardIndex Index of the card in the player's hand.
 * @return 1 on success, 0 on failure.
 */
int try_play_card(GameState *g, int playerIndex, int cardIndex) {
    if (!g || playerIndex < 0 || playerIndex >= g->num_players) return 0;
    
    Player* p = &g->players[playerIndex];
    
    if (cardIndex < 0 || cardIndex >= p->hand.size) return 0;
    
    Card cardToPlay = p->hand.cards[cardIndex];
    
    if (g->discard_pile.size == 0) return 0;
    Card topCard = g->discard_pile.cards[g->discard_pile.size - 1];

    if (is_move_valid(g->active_color, cardToPlay, topCard)) {
        Card played = remove_at(&p->hand, cardIndex);
        push_card(&g->discard_pile, played);
        
        // Update active color based on played card
        if (played.color != CARD_BLACK) {
            g->active_color = (int)played.color;
        }
        // Note: If black, active_color should be set by a following action (choose color)
        
        printf("Carte jouee ! Reste : %d cartes\n", p->hand.size);
        return 1;
    }
    
    printf("Coup invalide !\n");
    return 0;
}

/**
 * @brief Handles drawing a card for a player, including recycling the discard pile if needed.
 * @param g Pointer to the GameState.
 * @param playerIndex Index of the player drawing.
 * @return 1 on success, 0 if no cards are available.
 */
int player_draw_card(GameState *g, int playerIndex) {
    if (!g || playerIndex < 0 || playerIndex >= g->num_players) return 0;

    Player* p = &g->players[playerIndex];

    if (g->draw_pile.size == 0) {
        if (g->discard_pile.size <= 1) {
            printf("Plus aucune carte nulle part !\n");
            return 0; 
        }

        printf("Pioche vide ! On melange le talon...\n");
        
        // Keep the top card
        Card topVisible = pop_card(&g->discard_pile);
        
        // Move rest to draw pile
        while(g->discard_pile.size > 0) {
            push_card(&g->draw_pile, pop_card(&g->discard_pile));
        }
        
        // Put the visible card back
        push_card(&g->discard_pile, topVisible);
        
        shuffle_deck(&g->draw_pile);
    }

    draw_to_hand(p, &g->draw_pile);
    return 1;
}
