/**
    @file game.h
    @author Charles CAILLON
    @date 2026-03-18
    @date 2026-04-14
    @brief Header file for the main game logic and state management of King for Four.
*/
#ifndef GAME_H
#define GAME_H

#include "player.h"

/**
 * @struct KingForFourGameState_St
 * @brief Represents the complete state of a game session.
 */
typedef struct {
    Player_St players[4];    /**< Array of players in the game */
    int numPlayers;      /**< Number of active players */
    int currentPlayer;   /**< Index of the player whose turn it is (0-3) */
    int gameDirection;   /**< Direction of play (1 for clockwise, -1 for counter-clockwise) */
    int activeColor;     /**< The color currently in play, especially after a Joker (0-3) */
    Deck_St drawPile;       /**< The pile of cards to draw from */
    Deck_St discardPile;    /**< The pile of cards already played */
} KingForFourGameState_St;

// --- PROTOTYPES ---

/**
    @brief Initializes the game logic and state.
    
    @param g Pointer to the KingForFourGameState_St to initialize.
*/
void kingForFour_initGameLogic(KingForFourGameState_St* g);

/**
    @brief Validates if a move is legal according to Uno rules.

    @param[in]     active_color The current active color.
    @param[in]     played       The card the player wants to play.
    @param[in]     top          The card currently on top of the discard pile.
    @return                     1 if the move is valid, 0 otherwise.
*/
int kingForFour_isMoveValid(int active_color, Card_St played, Card_St top);

/**
    @brief Distributes initial cards to players and sets up the discard pile.

    @param[in,out] g Pointer to the GameState_St.
*/
void kingForFour_distributeCards(KingForFourGameState_St* g);

/**
    @brief Attempts to play a card from a player's hand.

    @param[in,out] g           Pointer to the GameState_St.
    @param[in]     playerIndex Index of the player playing the card.
    @param[in]     cardIndex   Index of the card in the player's hand.
    @return                    1 if the card was successfully played, 0 otherwise.
*/
int kingForFour_tryPlayCard(KingForFourGameState_St *g, int playerIndex, int cardIndex);

/**
    @brief Makes a player draw a card from the draw pile.

    @param[in,out] g           Pointer to the GameState_St.
    @param[in]     playerIndex Index of the player drawing the card.
    @return                    1 if a card was drawn, 0 if no cards are available.
*/
int kingForFour_playerDrawCard(KingForFourGameState_St *g, int playerIndex);

#endif // GAME_H
