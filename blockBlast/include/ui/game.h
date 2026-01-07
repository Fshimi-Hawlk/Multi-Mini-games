/**
 * @file game.h (ui)
 * @author Fshimi Hawlk
 * @date 2026-01-07
 * @brief Main UI drawing entry point.
 */

#ifndef UI_GAME_H
#define UI_GAME_H

#include "utils/userTypes.h"

/**
 * @brief Draws all UI elements for the current scene.
 *
 * In GAME scene: renders the board and the three prefab slots (skipping placed ones).
 * In ALL_PREFABS scene: renders every prefab in the bag (used for debugging/visualization).
 *
 * @param game Pointer to the current game state.
 */
void drawUI(const GameState_St* const game);

#endif // UI_GAME_H