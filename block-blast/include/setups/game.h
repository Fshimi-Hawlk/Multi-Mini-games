/**
 * @file game.h (setups)
 * @author Fshimi Hawlk
 * @date 2026-01-07
 * @brief Full game state initialization.
 */

#ifndef SETUPS_GAME_H
#define SETUPS_GAME_H

/**
 * @brief Initializes the full game: RNG, window, fonts, prefabs, board, and state.
 *
 * Seeds PRNG, opens window, loads fonts, sets up board position, fills prefab bags,
 * shuffles initial slots, and prepares camera/UI if needed.
 */
void initGame(void);

#endif // SETUPS_GAME_H