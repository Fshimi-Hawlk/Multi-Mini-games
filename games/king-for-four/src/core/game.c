/**
    @file game.c
    @author i-Charlys
    @date 2026-03-11
    @date 2026-04-14
    @brief Core game logic implementation for King for Four.
*/
#include "core/game.h"
#include <stdio.h>

// --- Public Functions ---

/**
 * @brief Initializes the game logic and state.
 * @param g Pointer to the KingForFourGameState_St to initialize.
 */
void kingForFour_initGameLogic(KingForFourGameState_St* g) {
    if (!g) return;
    g->currentPlayer = 0;
    g->gameDirection = 1;
    g->activeColor = -1;
    g->numPlayers = 0;
    
    kingForFour_clearDeck(&g->drawPile);
    kingForFour_clearDeck(&g->discardPile);
    
    for (int i = 0; i < 4; i++) {
        kingForFour_clearDeck(&g->players[i].hand);
    }
}

/**
 * @brief Checks if a move is valid based on color or value matching.
 * @param activeColor The current active color (set by special cards).
 * @param played The card being played.
 * @param top The card on top of the discard pile.
 * @return 1 if the move is valid, 0 otherwise.
 */
int kingForFour_isMoveValid(int activeColor, Card_St played, Card_St top) {
    if (played.color == CARD_BLACK) return 1; // Joker or +4 can always be played

    CardColor_Et currentMatchColor = (activeColor >= 0 && activeColor < 4) ? (CardColor_Et)activeColor : top.color;

    if (played.color == currentMatchColor) return 1;
    if (played.value == top.value) return 1;

    return 0;
}

/**
 * @brief Distributes initial cards to players and sets up the discard pile.
 * @param g Pointer to the KingForFourGameState_St.
 */
void kingForFour_distributeCards(KingForFourGameState_St* g) {
    if (!g || g->numPlayers == 0) return;

    // 1. Setup draw pile (Full Uno-like deck)
    kingForFour_clearDeck(&g->drawPile);
    for (int c = 0; c < 4; c++) {
        // Numbers 0-9
        for (int v = ZERO; v <= NINE; v++) {
            kingForFour_pushCard(&g->drawPile, (Card_St){(CardColor_Et)c, (CardValue_Et)v});
            if (v > ZERO) kingForFour_pushCard(&g->drawPile, (Card_St){(CardColor_Et)c, (CardValue_Et)v}); // Two of each 1-9
        }
        // Action cards (two of each)
        kingForFour_pushCard(&g->drawPile, (Card_St){(CardColor_Et)c, SKIP});
        kingForFour_pushCard(&g->drawPile, (Card_St){(CardColor_Et)c, SKIP});
        kingForFour_pushCard(&g->drawPile, (Card_St){(CardColor_Et)c, REVERSE});
        kingForFour_pushCard(&g->drawPile, (Card_St){(CardColor_Et)c, REVERSE});
        kingForFour_pushCard(&g->drawPile, (Card_St){(CardColor_Et)c, PLUS_TWO});
        kingForFour_pushCard(&g->drawPile, (Card_St){(CardColor_Et)c, PLUS_TWO});
    }
    // Wild cards (four of each)
    for (int i = 0; i < 4; i++) {
        kingForFour_pushCard(&g->drawPile, (Card_St){CARD_BLACK, JOKER});
        kingForFour_pushCard(&g->drawPile, (Card_St){CARD_BLACK, PLUS_FOUR});
    }

    kingForFour_shuffleDeck(&g->drawPile);

    // 2. Deal hands
    for (int i = 0; i < g->numPlayers; i++) {
        for (int j = 0; j < 7; j++) {
            kingForFour_drawToHand(&g->players[i], &g->drawPile);
        }
    }

    // 3. Initial discard
    if (g->drawPile.size > 0) {
        Card_St first = kingForFour_popCard(&g->drawPile);
        kingForFour_pushCard(&g->discardPile, first);
        
        // Setup initial game state based on first card
        if (first.color == CARD_BLACK) {
            g->activeColor = -1; // No color chosen yet, host must pick
        } else {
            g->activeColor = (int)first.color;
        }

        // Apply first card effect logic
        int skip = 0;
        if (first.value == SKIP) skip = 1;
        if (first.value == PLUS_TWO) {
            kingForFour_playerDrawCard(g, 0); // Next player (index 0 for start) draws 2
            kingForFour_playerDrawCard(g, 0);
            skip = 1;
        }
        if (first.value == REVERSE) {
            if (g->numPlayers == 2) skip = 1;
            else g->gameDirection *= -1;
        }
        
        // Adjust starting player
        int offset = skip * g->gameDirection;
        g->currentPlayer = (offset % g->numPlayers + g->numPlayers) % g->numPlayers;
    }
}

/**
 * @brief Attempts to play a card from a player's hand.
 * @param g Pointer to the KingForFourGameState_St.
 * @param playerIndex Index of the player playing.
 * @param cardIndex Index of the card in the player's hand.
 * @return 1 on success, 0 on failure.
 */
int kingForFour_tryPlayCard(KingForFourGameState_St *g, int playerIndex, int cardIndex) {
    if (!g || playerIndex < 0 || playerIndex >= g->numPlayers) return 0;
    
    Player_St* p = &g->players[playerIndex];
    
    if (cardIndex < 0 || cardIndex >= p->hand.size) return 0;
    
    Card_St cardToPlay = p->hand.cards[cardIndex];
    
    if (g->discardPile.size == 0) return 0;
    Card_St topCard = g->discardPile.cards[g->discardPile.size - 1];

    if (kingForFour_isMoveValid(g->activeColor, cardToPlay, topCard)) {
        Card_St played = kingForFour_removeAt(&p->hand, cardIndex);
        kingForFour_pushCard(&g->discardPile, played);
        
        // Update active color based on played card
        if (played.color != CARD_BLACK) {
            g->activeColor = (int)played.color;
        }
        // Note: If black, activeColor should be set by a following action (choose color)
        
        printf("Carte jouee ! Reste : %d cartes\n", p->hand.size);
        return 1;
    }
    
    printf("Coup invalide !\n");
    return 0;
}

/**
 * @brief Handles drawing a card for a player, including recycling the discard pile if needed.
 * @param g Pointer to the KingForFourGameState_St.
 * @param playerIndex Index of the player drawing.
 * @return 1 on success, 0 if no cards are available.
 */
int kingForFour_playerDrawCard(KingForFourGameState_St *g, int playerIndex) {
    if (!g || playerIndex < 0 || playerIndex >= g->numPlayers) return 0;

    Player_St* p = &g->players[playerIndex];

    if (g->drawPile.size == 0) {
        if (g->discardPile.size <= 1) {
            printf("Plus aucune carte nulle part !\n");
            return 0; 
        }

        printf("Pioche vide ! On melange le talon...\n");
        
        // Keep the top card
        Card_St topVisible = kingForFour_popCard(&g->discardPile);
        
        // Move rest to draw pile
        while(g->discardPile.size > 0) {
            kingForFour_pushCard(&g->drawPile, kingForFour_popCard(&g->discardPile));
        }
        
        // Put the visible card back
        kingForFour_pushCard(&g->discardPile, topVisible);
        
        kingForFour_shuffleDeck(&g->drawPile);
    }

    kingForFour_drawToHand(p, &g->drawPile);
    return 1;
}
