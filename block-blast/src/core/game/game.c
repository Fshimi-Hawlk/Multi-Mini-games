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
    // 1. Calculate how much we want to shift the distribution right/left
    f32 shift = 0.0f;

    // Streak influence - main driver (positive = shift right = bigger shapes)
    shift += game->streakCount * 0.04f;         // +0.04 per streak level

    // Empty cells influence - crowded board = prefer smaller
    u32 emptyCells = getEmptyCellCount(&game->board);

    f32 fullnessRatio = 1.0f - (emptyCells / (f32) (game->board.width * game->board.height));
    shift -= fullnessRatio * 0.85f;                 // up to +0.12 when almost full

    // Recent performance (this turn's score gain)
    f32 placementScore = 0;
    for (u8 i = 0; i < 3; ++i) {
        placementScore += game->slots->prefab->blockCount * SCORE_PER_UNIT_PLACED;
    }

    f32 performanceBonus = scoreDelta - placementScore;
    f32 perfFactor = performanceBonus / (placementScore * 2.5f);
    shift += perfFactor;

    // Optional cap - don't go too extreme
    shift = clamp(shift, -0.35f, 0.60f);

    // 2. Apply shift: move mass from low → high sizes
    // We use a simple linear transfer pattern
    const f32 lowEnd[]   = {0,1,2,3};
    const f32 highEnd[]  = {4,5,6,7,8};

    f32 streakFactor = 0.3f + game->streakCount * 0.07f;

    // how much mass we move (not all!)
    f32 totalTransfer = fabsf(shift) * (streakFactor + 0.4f * perfFactor) * fullnessRatio;
    totalTransfer = min(totalTransfer, 0.82f);

    if (shift > 0) {
        // Shift right: low → high
        f32 amountPerLow = totalTransfer / 4.0f;  // 4 low sizes
        for (u32 i = 0; i < 4; i++) {
            u32 s = lowEnd[i];
            f32 take = min(game->sizeWeights.weights[s], amountPerLow);
            game->sizeWeights.weights[s] -= take;
            // Distribute to high end proportionally
            for (u32 j = 0; j < 5; j++) {
                u32 hs = highEnd[j];
                if (game->sizeWeights.baseWeights[hs] > 0) {  // only to existing types
                    game->sizeWeights.weights[hs] += take * 0.2f;
                }
            }
        }
    } else if (shift < 0) {
        // Shift left: high → low (help player)
        f32 amountPerLigh = totalTransfer / 5.0f;
        for (u32 i = 0; i < 5; i++) {
            u32 s = highEnd[i];
            f32 take = min(game->sizeWeights.weights[s], amountPerLigh);
            game->sizeWeights.weights[s] -= take;
            for (u32 j = 0; j < 4; j++) {
                u32 ls = lowEnd[j];
                game->sizeWeights.weights[ls] += take * 0.25f;
            }
        }
    }

    // 3. Clamp & normalize
    f32 sum = 0.0f;
    for (u32 i = 0; i < MAX_SHAPE_SIZE; i++) {
        game->sizeWeights.weights[i] = max(0.0f, game->sizeWeights.weights[i]);
        sum += game->sizeWeights.weights[i];
    }

    if (sum > 0.0001f) {
        for (u32 i = 0; i < MAX_SHAPE_SIZE; i++) {
            game->sizeWeights.weights[i] /= sum;
        }
    } else { // catastrophic case - keep base weights
        for (u32 i = 0; i < MAX_SHAPE_SIZE; i++) {
            game->sizeWeights.weights[i] = game->sizeWeights.baseWeights[i];
        }
    }
}