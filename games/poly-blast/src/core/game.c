/**
    @file game.c
    @author Kimi BERGE
    @date 2026-01-07
    @date 2026-04-14
    @brief Score management functions implementation.
*/
#include "core/game.h"
#include "core/board.h"
#include "core/placement.h"

#include "sharedUtils/container.h"
#include "sharedUtils/mathUtils.h"

#define da_copy(da, copy)  \
    do {                   \
        (copy)->count = 0; \
        da_append_many(    \
            (copy),        \
            (da)->items,   \
            (da)->count    \
        );                 \
    } while (0)

/**
    @brief Creates a deep copy of a PrefabManager_St.

    @param[in]     manager      Source manager to copy.
    @return                     Independent deep copy of the prefab manager.
*/
PrefabManager_St polyBlast_deepcopyPrefabManager(const PrefabManager_St* const manager) {
    PrefabManager_St copy = {0};
    memcpy(&copy.sizeWeights, &manager->sizeWeights, sizeof(SizeWeight_St));

    for (u8 i = 0; i < MAX_SHAPE_SIZE; i++) {
        copy.bags[i].count = 0;
        if (manager->bags[i].count == 0) continue;
        da_append_many(&copy.bags[i], manager->bags[i].items, manager->bags[i].count);
    }

    return copy;
}

/**
    @brief Finds all valid anchor positions where a shape can be placed on the board.

    @param[in]     board        Pointer to the board.
    @param[in]     shape        Pointer to the shape to test.
    @return                     Vector of valid anchor positions.
*/
AnchorVec_St polyBlast_getAnchorCandidates(const Board_St* const board, const Shape_St* const shape) {
    AnchorVec_St anchors = {0};

    if (board == NULL || shape == NULL || shape->prefab == NULL) return anchors;

    const u8 maxY = board->height - shape->prefab->height;
    const u8 maxX = board->width - shape->prefab->width;

    for (u8 y = 0; y <= maxY; ++y) {
        for (u8 x = 0; x <= maxX; ++x) {
            u8Vector2 anchor = {x, y};
            if (polyBlast_isShapePlaceable(shape, castTo(s8Vector2) &anchor, board)) {
                da_append(&anchors, anchor);
            }
        }
    }

    return anchors;
}

/**
    @brief Tests if any of the shapes in the slots can be placed on the board.

    @param[in]     board        Current board state (copied by value for simulation).
    @param[in]     slots        The three active prefab slots.
    @return                     true if no shapes can be placed (Game Over), false otherwise.
*/
bool polyBlast_testGameOver(Board_St board, const ShapeSlots_t slots) {
    static const u8 permutations[6][3] = {
        {0, 1, 2}, {0, 2, 1},
        {1, 0, 2}, {1, 2, 0},
        {2, 0, 1}, {2, 1, 0}
    };

    bool allPlaced = true;
    Board_St simBoard = board;

    for (u8 p = 0; p < 6; p++) {
        allPlaced = polyBlast_canPlaceAll(&simBoard, slots, permutations[p], 0);

        if (allPlaced) break;
    }

    return !allPlaced;
}

/**
    @brief Builds the score and streak text strings.

    @param[in,out] scoringState Pointer to the scoring state to update.
*/
void polyBlast_buildScoreRelatedTexts(ScoringState_St* const scoringState) {
    // build score text
    snprintf(scoringState->scoreText, sizeof(scoringState->scoreText), "Score: %lu", scoringState->score);

    // build streak text
    snprintf(scoringState->streakText, sizeof(scoringState->streakText), "Streak: %u", scoringState->streakCount);
}

/**
    @brief Computes score increment from the latest placement.

    Scoring rules (as currently implemented):
      - Base: SCORE_PER_LINE_CLEAR per cleared line (row or column)
      - Multiplier: 1.0 + 0.5 × (number of lines cleared - 1) when ≥ 2 lines

    @param[in]     board        Board after placement but before clearBoard() is called.
    @return                     Score to add from line clears only.
*/
static f32 calculateBoardClearingScore(const Board_St* const board) {
    u8 linesCleared = 0;
    for (u8 row = 0; row < board->width; ++row) {
        linesCleared += board->rowsToClear[row];
    }

    for (u8 col = 0; col < board->height; ++col) {
        linesCleared += board->columnsToClear[col];
    }

    f32 multiBonus = linesCleared > 1 ? 1.0f + 0.5f * (linesCleared - 1) : 1.0f;
    return linesCleared * SCORE_PER_LINE_CLEAR * multiBonus;
}

/**
    @brief Manages score and streak updates after a shape is placed.

    @param[in,out] scoringState Pointer to the scoring state.
    @param[in]     board        Pointer to the board.
    @param[in]     prefabBlockCount Number of blocks in the placed prefab.
*/
void polyBlast_manageScoreAndStreak(ScoringState_St* const scoringState, const Board_St* const board, const u8 prefabBlockCount) {
    scoringState->score += prefabBlockCount * SCORE_PER_UNIT_PLACED;

    if (polyBlast_checkBoardForClearing(board)) {
        scoringState->streakCount++;
        scoringState->streakGrace = (scoringState->streakCount + 1) / 2;

        scoringState->score += calculateBoardClearingScore(board) * scoringState->streakCount;
    } else if (scoringState->streakGrace > 0) {
        scoringState->streakGrace--;

        if (scoringState->streakGrace == 0) {
            scoringState->streakCount = 0;
        }
    }

    polyBlast_buildScoreRelatedTexts(scoringState);
}

/**
    @brief Adjusts the dynamic probability weights for prefab sizes.

    @param[in,out] game         Pointer to the current game state.
    @param[in]     scoreDelta   The score gained this turn.
*/
void polyBlast_adjustSizeWeights(GameState_St* const game, const f32 scoreDelta) {
    // ─────────────────────────────────────────────────────────────
    // Tuning constants

    // Streak influence
    const f32 STREAK_SHIFT_PER_LEVEL     = 0.04f;     // base shift per streak point
    const f32 STREAK_FACTOR_BASE         = 0.3f;      // minimum multiplier for transfer strength
    const f32 STREAK_FACTOR_PER_LEVEL    = 0.07f;     // extra per streak

    // Board fullness influence (negative shift = favor small shapes)
    const f32 FULLNESS_MAX_SHIFT         = 0.85f;     // max negative shift when board is full

    // Performance (this turn) influence
    const f32 PERF_FACTOR_DENOM          = 2.5f;      // divisor for performance ratio

    // Overall shift limits
    const f32 SHIFT_MIN                  = -0.85f;
    const f32 SHIFT_MAX                  =  0.60f;

    // Transfer strength caps and scaling
    const f32 TRANSFER_MAX               = 0.82f;    // never move more than this fraction at once
    const f32 TRANSFER_PERF_WEIGHT       = 0.4f;     // how much this-turn perf affects transfer

    // Recovery boost when board is crowded (helps small shapes come back)
    const f32 FULLNESS_RECOVERY_THRESHOLD  = 0.75f;    // board fullness above this -> stronger left shift
    const f32 FULLNESS_RECOVERY_MULTIPLIER = 1.5f;     // how much extra mass to move when crowded

    // Minimum weight floors (already in your code, but centralized here)
    const f32 MIN_WEIGHT_IF_ENABLED      = 0.01f;     // for sizes with base > 0

    // ─────────────────────────────────────────────────────────────
    // 1. Shift calculation
    f32 shift = 0.0f;

    // Streak -> bigger shapes
    shift += game->scoring.streakCount * STREAK_SHIFT_PER_LEVEL;

    // Fullness -> smaller shapes when crowded
    u32 emptyCells = polyBlast_getEmptyCellCount(&game->board);
    f32 fullnessRatio = 1.0f - (emptyCells / (f32) (game->board.width * game->board.height));
    shift -= fullnessRatio * FULLNESS_MAX_SHIFT;

    // This turn's performance
    f32 placementScore = 0;
    for (u8 i = 0; i < 3; ++i) {
        placementScore += game->prefabManager.slots[i].prefab->blockCount * SCORE_PER_UNIT_PLACED;
    }

    f32 performanceBonus = scoreDelta - placementScore;
    f32 perfFactor = performanceBonus / (placementScore * PERF_FACTOR_DENOM);
    shift += perfFactor;

    // Hard clamp shift
    shift = clamp(shift, SHIFT_MIN, SHIFT_MAX);

    // ─────────────────────────────────────────────────────────────
    // 2. Transfer: Apply shift => move mass from low -> high sizes
    // We use a simple linear transfer pattern
    // and make sure we never take below floor during move
    const f32 lowEnd[]   = {0,1,2,3};
    const f32 highEnd[]  = {4,5,6,7,8};

    f32 streakFactor = STREAK_FACTOR_BASE + game->scoring.streakCount * STREAK_FACTOR_PER_LEVEL;
    f32 totalTransfer = fabsf(shift) 
                      * (streakFactor + TRANSFER_PERF_WEIGHT * perfFactor)
                      * fullnessRatio;
    
    totalTransfer = min(totalTransfer, TRANSFER_MAX);

    if (shift > 0) { // Right shift: low -> high
        f32 amountPerLow = totalTransfer / 4.0f;
        for (u32 i = 0; i < 4; i++) {
            u32 s = lowEnd[i];

            // Protect floor during take
            f32 floor = game->prefabManager.sizeWeights.baseWeights[s] == 0.0f ? 0.0f : MIN_WEIGHT_IF_ENABLED;
            f32 available = max(0.0f, game->prefabManager.sizeWeights.runTimeWeights[s] - floor);
            f32 take = min(available, amountPerLow);
            game->prefabManager.sizeWeights.runTimeWeights[s] -= take;

            // Distribute only to sizes that exist
            f32 amountPerHigh = take * 0.2f;
            for (u32 j = 0; j < 5; j++) {
                u32 hs = highEnd[j];
                if (game->prefabManager.sizeWeights.baseWeights[hs] > 0.0f) {
                    game->prefabManager.sizeWeights.runTimeWeights[hs] += amountPerHigh;
                }
            }
        }
    } else if (shift < 0) { // Left shift: high -> low
        f32 amountPerHigh = totalTransfer / 5.0f;

        // Boost recovery when board is very full
        if (fullnessRatio > FULLNESS_RECOVERY_THRESHOLD) {
            amountPerHigh *= FULLNESS_RECOVERY_MULTIPLIER;
        }

        for (u32 i = 0; i < 5; i++) {
            u32 s = highEnd[i];
            f32 floor = game->prefabManager.sizeWeights.baseWeights[s] == 0.0f ? 0.0f : MIN_WEIGHT_IF_ENABLED;
            f32 available = max(0.0f, game->prefabManager.sizeWeights.runTimeWeights[s] - floor);
            f32 take = min(available, amountPerHigh);
            game->prefabManager.sizeWeights.runTimeWeights[s] -= take;

            f32 amountPerLow = take * 0.25f;
            for (u32 j = 0; j < 4; j++) {
                u32 ls = lowEnd[j];
                game->prefabManager.sizeWeights.runTimeWeights[ls] += amountPerLow;
            }
        }
    }

    // ─────────────────────────────────────────────────────────────
    // 3. Clamp & normalize
    f32 sum = 0.0f;
    for (u32 i = 0; i < MAX_SHAPE_SIZE; i++) {
        f32 min = game->prefabManager.sizeWeights.baseWeights[i] == 0.0f ? 0.0f : MIN_WEIGHT_IF_ENABLED;
        game->prefabManager.sizeWeights.runTimeWeights[i] = max(min, game->prefabManager.sizeWeights.runTimeWeights[i]);
        sum += game->prefabManager.sizeWeights.runTimeWeights[i];
    }

    if (sum > 0.0001f) {
        for (u32 i = 0; i < MAX_SHAPE_SIZE; i++) {
            game->prefabManager.sizeWeights.runTimeWeights[i] /= sum;
        }
    } else { // Fallback
        for (u32 i = 0; i < MAX_SHAPE_SIZE; i++) {
            game->prefabManager.sizeWeights.runTimeWeights[i] = game->prefabManager.sizeWeights.baseWeights[i];
        }
    }
}