/**
 * @file game.h (core/game)
 * @author Fshimi Hawlk
 * @date 2026-01-07
 * @brief Score management functions for the game.
 */

#ifndef CORE_GAME_GAME_H
#define CORE_GAME_GAME_H

#include "utils/userTypes.h"

/**
 * @brief Builds the score and streak texts string based on the current game state.
 */
void buildScoreRelatedTexts(void);

/**
 * @brief Calculates the score based on the board's current state.
 *
 * @param board Pointer to the board structure.
 * @return The calculated score as a float.
 */
f32 calculateScore(const Board_St* const board);

/**
 * @brief Manages and updates the game score using the provided prefab.
 *
 * @param game Pointer to the game state structure.
 * @param prefab Pointer to the prefab structure.
 */
void manageScore(GameState_St* const game, const Prefab_St* const prefab);

void adjustSizeWeights(GameState_St* const game, const f32 scoreDelta);

#endif // CORE_GAME_GAME_H