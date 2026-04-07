/**
 * @file puissance4API.h
 * @brief Public API for the Puissance4 (Connect Four) mini-game
 * @author Maxime CHAUVEAU
 * @date March 2026
 * @version 1.0.0
 * 
 * @section overview Overview
 * 
 * Puissance4 (also known as Connect Four) is a two-player strategy game
 * where players take turns dropping colored discs into a vertical grid.
 * The first player to connect four discs of their color in a row wins.
 * 
 * @section game_rules Game Rules
 * - The game is played on a 6-row by 7-column grid
 * - Players alternate turns, with Player 1 (Red) going first
 * - Players drop discs into any of the 7 columns
 * - Discs fall to the lowest available position in the column
 * - The first player to connect 4 discs horizontally, vertically, or diagonally wins
 * - If the board is filled without a winner, the game is a draw
 * 
 * @section usage Usage
 * 
 * @code
 * Puissance4Game_St* game = puissance4_initGame(NULL);  // NULL = default config
 * if (!game) { handle error }
 * while (puissance4_isRunning(game)) {
 *     puissance4_gameLoop(game);
 * }
 * puissance4_freeGame(game);
 * @endcode
 * 
 * @section controls Controls
 * - Mouse Left Click: Drop piece in selected column
 * - ESC: Quit game
 * - SPACE: Restart game (when game is over)
 * 
 * @section dependencies Dependencies
 * - RayLib (graphics library)
 * - Standard C library
 */

#ifndef PUISSANCE4_API_H
#define PUISSANCE4_API_H

#include "APIs/gameError.h"
#include "APIs/gameConfig.h"
#include <stdbool.h>

typedef struct Puissance4Game_St Puissance4Game_St;

/**
 * @brief Initialize a new Puissance4 game
 * 
 * Allocates and initializes a new game instance with the given configuration.
 * If config is NULL, default values are used.
 * 
 * @param config Game configuration (can be NULL for default)
 * @return Pointer to the game instance, or NULL on failure
 * 
 * @post Game is ready to run with empty board
 * @post Current player is set to Player 1 (Red)
 */
Puissance4Game_St* puissance4_initGame(const GameConfig_St* config);

/**
 * @brief Run one frame of the game loop
 * 
 * Handles input, updates game state, and renders the current frame.
 * Should be called once per frame (typically 60 times per second).
 * 
 * @param game Game instance
 * 
 * @note This function handles all input processing internally
 * @note Rendering is performed using RayLib
 */
void puissance4_gameLoop(Puissance4Game_St* const game);

/**
 * @brief Free the game resources
 * 
 * Deallocates all memory associated with the game instance.
 * Safe to call with NULL pointer (no operation).
 * 
 * @param game Game instance to free
 * 
 * @post Game pointer is invalidated and should not be used
 */
void puissance4_freeGame(Puissance4Game_St* game);

/**
 * @brief Check if the game is still running
 * 
 * Returns the current running state of the game.
 * The game ends when a player wins, there's a draw, or the user quits.
 * 
 * @param game Game instance
 * @return true if game is running, false otherwise
 * 
 * @pre game must not be NULL
 */
bool puissance4_isRunning(const Puissance4Game_St* game);

#endif // PUISSANCE4_API_H
