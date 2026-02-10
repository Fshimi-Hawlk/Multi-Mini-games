/**
 * @file game.h (setups)
 * @author Fshimi Hawlk
 * @date 2026-01-07
 * @brief Full game state initialization.
 */

#ifndef SETUPS_GAME_H
#define SETUPS_GAME_H

#include "utils/userTypes.h"

/**
 * @brief Initializes the game: prefabs, board, and state.
 *
 * Sets up board, fills prefab bags, shuffles initial slots.
 */
void initGame(GameState_St* const game);

#endif // SETUPS_GAME_H