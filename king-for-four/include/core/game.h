/**
 * @file game.h
 * @author i-Charlys
 * @date 2026-03-18
 * @brief Header file for the main game logic and state management of King for Four.
 */

#ifndef GAME_H
#define GAME_H

#include "player.h"

/**
 * @struct GameState
 * @brief Represents the complete state of a game session.
 */
typedef struct {
    Player players[4];    /**< Array of players in the game */
    int num_players;      /**< Number of active players */
    int current_player;   /**< Index of the player whose turn it is (0-3) */
    int game_direction;   /**< Direction of play (1 for clockwise, -1 for counter-clockwise) */
    int active_color;     /**< The color currently in play, especially after a Joker (0-3) */
    Deck draw_pile;       /**< The pile of cards to draw from */
    Deck discard_pile;    /**< The pile of cards already played */
} GameState;

// --- PROTOTYPES ---

/**
 * @brief Initializes the game logic and state.
 * @param g Pointer to the GameState to initialize.
 */
void init_game_logic(GameState* g);

/**
 * @brief Validates if a move is legal according to Uno rules.
 * @param active_color The current active color.
 * @param played The card the player wants to play.
 * @param top The card currently on top of the discard pile.
 * @return 1 if the move is valid, 0 otherwise.
 */
int is_move_valid(int active_color, Card played, Card top);

/**
 * @brief Distributes initial cards to players and sets up the discard pile.
 * @param g Pointer to the GameState.
 */
void distribute_cards(GameState* g);

/**
 * @brief Attempts to play a card from a player's hand.
 * @param g Pointer to the GameState.
 * @param playerIndex Index of the player playing the card.
 * @param cardIndex Index of the card in the player's hand.
 * @return 1 if the card was successfully played, 0 otherwise.
 */
int try_play_card(GameState *g, int playerIndex, int cardIndex);

/**
 * @brief Makes a player draw a card from the draw pile.
 * @param g Pointer to the GameState.
 * @param playerIndex Index of the player drawing the card.
 * @return 1 if a card was drawn, 0 if no cards are available.
 */
int player_draw_card(GameState *g, int playerIndex);

#endif // GAME_H
