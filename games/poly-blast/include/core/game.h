/**
    @file game.h
    @author Kimi BERGE
    @date 2026-01-07
    @date 2026-04-14
    @brief Score management and high-level game logic functions.
*/
#ifndef CORE_GAME_GAME_H
#define CORE_GAME_GAME_H

#include "utils/userTypes.h"

/**
    @brief Creates a deep copy of a PrefabManager_St.

    A plain `memcpy` is insufficient because each `bags[i]` contains a heap-allocated
    `items` array. This function copies the struct and then re-creates every bag
    with its indices.

    @param[in]     manager      Source manager to copy.
    @return                     Independent deep copy of the prefab manager.
*/
PrefabManager_St polyBlast_deepcopyPrefabManager(const PrefabManager_St* const manager);

/**
    @brief Determines whether the current three slots can all be placed on the
           board in at least one ordering.

    Tries all 6 permutations of placement order. Uses an arena snapshot/rewind
    so the temporary `emptyCells` vector does not leak.

    @param[in]     board        Current board state.
    @param[in]     slots        The three active prefab slots.
    @return                     true if game over (no legal way to place all three shapes).
    @return                     false if at least one ordering works.
*/
bool polyBlast_testGameOver(Board_St board, const ShapeSlots_t slots);

/**
    @brief Builds the score and streak text strings based on the current game state.

    @param[in,out] scoringState Pointer to the scoring state to update.
*/
void polyBlast_buildScoreRelatedTexts(ScoringState_St* const scoringState);

/**
    @brief Calculates the score of the move and updates the streak.

    Adds SCORE_PER_UNIT_PLACED × blockCount, plus cleared lines calculated score.

    @param[in,out] scoringState Pointer to the scoring state.
    @param[in]     board        Pointer to the board.
    @param[in]     prefabBlockCount Number of blocks in the placed prefab.
*/
void polyBlast_manageScoreAndStreak(ScoringState_St* const scoringState, const Board_St* const board, const u8 prefabBlockCount);

/**
    @brief Adjusts the dynamic probability weights for prefab sizes based on game state.

    This function modifies the `game->sizeWeights.weights` array to shift the distribution
    of prefab sizes toward larger or smaller shapes depending on several factors:
      - Current streak count (longer streaks favor larger shapes)
      - Board fullness (crowded board favors smaller shapes)
      - Recent performance (this turn's score gain vs expected placement score)

    @param[in,out] game         Pointer to the current game state.
    @param[in]     scoreDelta   The actual score gained this turn.
*/
void polyBlast_adjustSizeWeights(PolyBlastGame_St* const game, const f32 scoreDelta);

#endif // CORE_GAME_GAME_H