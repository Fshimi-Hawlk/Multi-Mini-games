/**
    @file game.h (core/game)
    @author Fshimi Hawlk
    @date 2026-01-07
    @brief Score management functions for the game.
*/

#ifndef CORE_GAME_GAME_H
#define CORE_GAME_GAME_H

#include "utils/userTypes.h"

/**
    @brief Creates a deep copy of a PrefabManager_St.

    A plain `memcpy` is insufficient because each `bags[i]` contains a heap-allocated
    `items` array. This function copies the struct and then re-creates every bag
    with its indices.

    @param[in] manager  Source manager.
    @return             Independent deep copy. `prefabsBag` pointer and slot
                        prefabs remain valid as long as the original bag lives.
*/
PrefabManager_St deepcopyPrefabManager(const PrefabManager_St* const manager);

/**
    @brief Determines whether the current three slots can all be placed on the
           board in at least one ordering.

    Tries all 6 permutations of placement order. Uses an arena snapshot/rewind
    so the temporary `emptyCells` vector does not leak.

    @param[in] board  Current board state.
    @param[in] slots  The three active prefab slots.

    @return `true`  -> game over (no legal way to place all three shapes)
            `false` -> at least one ordering works.
*/
bool testGameOver(Board_St board, const ShapeSlots_t slots);

/**
    @brief Builds the score and streak texts string based on the current game state.
*/
void buildScoreRelatedTexts(ScoringState_St* const scoringState);

/**
    @brief Calculate the score of the move and update the streak relevant to any clearing of the board.

    Adds SCORE_PER_UNIT_PLACED × blockCount, plus cleared lines calculated score.

    @param game Pointer to the game.
    @param prefabBlockCount Block count in the placed prefab.
*/
void manageScoreAndStreak(ScoringState_St* const scoringState, const Board_St* const board, const u8 prefabBlockCount);

/**
    @brief Adjusts the dynamic probability weights for prefab sizes based on game state.

    This function modifies the `game->sizeWeights.weights` array to shift the distribution
    of prefab sizes toward larger or smaller shapes depending on several factors:
      - Current streak count (longer streaks favor larger shapes)
      - Board fullness (crowded board favors smaller shapes)
      - Recent performance (this turn's score gain vs expected placement score)

    The algorithm works in three main phases:

    1. **Compute net shift direction and strength**
       - Positive shift = favor larger shapes (move probability mass from low to high sizes)
       - Negative shift = favor smaller shapes (move mass from high to low sizes)
       - Sources of shift:
         - Streak: strong positive driver
         - Fullness ratio: strong negative driver when board is nearly full
         - Performance delta: small adjustment based on whether player over/under-performed

    2. **Transfer probability mass**
       - Calculates how much total mass to move (`totalTransfer`)
       - Protects minimum floors during transfer (sizes cannot go below 0.01 if enabled, or 0 if disabled)
       - When shifting right (larger shapes): takes from sizes 0-3, distributes to sizes 4-8 (only if base > 0)
       - When shifting left (smaller shapes): takes from sizes 4-8, gives to sizes 0-3
       - Boosts left-shift transfer amount when board is very crowded (>75% full) to help recovery

    3. **Enforce floors and normalize**
       - Applies per-size floor: 0.0f if baseWeights[i] == 0 (disabled size), else MIN_WEIGHT_IF_ENABLED
       - Normalizes so weights sum to 1.0f
       - Fallback: if sum becomes invalid (< 0.0001f), resets to baseWeights

    @param game Pointer to the current game state. Must have valid sizeWeights and board data.
    @param scoreDelta The actual score gained this turn (used to compute performance factor).
                      Typically passed from the scoring function after placement and clearing.

    @note
    - All magic numbers are extracted as named constants at the top for easy tuning.
    - The function never revives disabled sizes (baseWeights[i] == 0.0f stays 0.0f).
    - Floors are respected during mass transfer, not only at the end, to prevent starvation.
    - Called before generating the next set of prefabs (e.g. in shuffleSlots or prefab generation).
*/
void adjustSizeWeights(GameState_St* const game, const f32 scoreDelta);

#endif // CORE_GAME_GAME_H