/**
 * @file game.h (setups)
 * @author Fshimi Hawlk
 * @date 2026-01-07
 * @brief Full game state initialization.
 */

#ifndef SETUPS_GAME_H
#define SETUPS_GAME_H

/**
 * @brief Initializes the game state and related resources.
 *
 * Sets up the board (clears it, sets position and dimensions), initializes the three prefab slots,
 * resets score and streak, fills the prefab bag if needed, shuffles the initial slots,
 * and performs any other one-time game setup.
 */
void initGame(void);

#endif // SETUPS_GAME_H