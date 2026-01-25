/**
 * @file game.h (core/game)
 * @author Fshimi Hawlk
 * @date 2026-01-07
 * @brief Score management functions implementatuion.
 */

#include "core/game/game.h"
#include "core/game/board.h"

#include "utils/globals.h"
#include "utils/utils.h"

void buildScoreRelatedTexts(void) {
    // build score text
    snprintf(game.scoreText, sizeof(game.scoreText), "Score: %lu", game.score);

    // build streak text
    snprintf(game.streakText, sizeof(game.streakText), "Streak: %u", game.streakCount);
}

f32 calculateScore(const Board_St* const board) {
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

void manageScore(GameState_St* const game, const Prefab_St* const prefab) {
    game->score += prefab->blockCount * SCORE_PER_UNIT_PLACED;

    if (checkBoardForClearing(&game->board)) {
        clearBoard(&game->board);

        game->streakCount++;
        game->streakPlacementResetCnt = 0;
        game->score += calculateScore(&game->board) * game->streakCount;
    } else {
        game->streakPlacementResetCnt++;
    }

    if (game->streakPlacementResetCnt >= (game->streakCount / 2.0f)) {
        game->streakCount = 0;
        game->streakPlacementResetCnt = 0;
    }

    buildScoreRelatedTexts();
}

void adjustSizeWeights(GameState_St* const game, const f32 scoreDelta) {
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
    const f32 FULLNESS_RECOVERY_THRESHOLD  = 0.75f;    // board fullness above this → stronger left shift
    const f32 FULLNESS_RECOVERY_MULTIPLIER = 1.5f;     // how much extra mass to move when crowded

    // Minimum weight floors (already in your code, but centralized here)
    const f32 MIN_WEIGHT_IF_ENABLED      = 0.01f;     // for sizes with base > 0

    // ─────────────────────────────────────────────────────────────
    // 1. Shift calculation
    f32 shift = 0.0f;

    // Streak → bigger shapes
    shift += game->streakCount * STREAK_SHIFT_PER_LEVEL;

    // Fullness → smaller shapes when crowded
    u32 emptyCells = getEmptyCellCount(&game->board);
    f32 fullnessRatio = 1.0f - (emptyCells / (f32) (game->board.width * game->board.height));
    shift -= fullnessRatio * FULLNESS_MAX_SHIFT;

    // This turn's performance
    f32 placementScore = 0;
    for (u8 i = 0; i < 3; ++i) {
        placementScore += game->slots->prefab->blockCount * SCORE_PER_UNIT_PLACED;
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

    f32 streakFactor = STREAK_FACTOR_BASE + game->streakCount * STREAK_FACTOR_PER_LEVEL;
    f32 totalTransfer = fabsf(shift) 
                      * (streakFactor + TRANSFER_PERF_WEIGHT * perfFactor)
                      * fullnessRatio;
    
    totalTransfer = min(totalTransfer, TRANSFER_MAX);

    if (shift > 0) { // Right shift: low -> high
        f32 amountPerLow = totalTransfer / 4.0f;
        for (u32 i = 0; i < 4; i++) {
            u32 s = lowEnd[i];

            // Protect floor during take
            f32 floor = game->sizeWeights.baseWeights[s] == 0.0f ? 0.0f : MIN_WEIGHT_IF_ENABLED;
            f32 available = max(0.0f, game->sizeWeights.weights[s] - floor);
            f32 take = min(available, amountPerLow);
            game->sizeWeights.weights[s] -= take;

            // Distribute only to sizes that exist
            f32 amountPerHigh = take * 0.2f;
            for (u32 j = 0; j < 5; j++) {
                u32 hs = highEnd[j];
                if (game->sizeWeights.baseWeights[hs] > 0.0f) {
                    game->sizeWeights.weights[hs] += amountPerHigh;
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
            f32 floor = game->sizeWeights.baseWeights[s] == 0.0f ? 0.0f : MIN_WEIGHT_IF_ENABLED;
            f32 available = max(0.0f, game->sizeWeights.weights[s] - floor);
            f32 take = min(available, amountPerHigh);
            game->sizeWeights.weights[s] -= take;

            f32 amountPerLow = take * 0.25f;
            for (u32 j = 0; j < 4; j++) {
                u32 ls = lowEnd[j];
                game->sizeWeights.weights[ls] += amountPerLow;
            }
        }
    }

    // ─────────────────────────────────────────────────────────────
    // 3. Clamp & normalize
    f32 sum = 0.0f;
    for (u32 i = 0; i < MAX_SHAPE_SIZE; i++) {
        f32 min = game->sizeWeights.baseWeights[i] == 0.0f ? 0.0f : MIN_WEIGHT_IF_ENABLED;
        game->sizeWeights.weights[i] = max(min, game->sizeWeights.weights[i]);
        sum += game->sizeWeights.weights[i];
    }

    if (sum > 0.0001f) {
        for (u32 i = 0; i < MAX_SHAPE_SIZE; i++) {
            game->sizeWeights.weights[i] /= sum;
        }
    } else { // Fallback
        for (u32 i = 0; i < MAX_SHAPE_SIZE; i++) {
            game->sizeWeights.weights[i] = game->sizeWeights.baseWeights[i];
        }
    }
}