/**
 * @file game.h (core/game)
 * @author Fshimi Hawlk
 * @date 2026-01-07
 * @brief Score management functions implementatuion.
 */

#include "core/game/game.h"
#include "core/game/board.h"

#include "utils/globals.h"

void buildScoreText(void) {
    snprintf(game.scoreText, sizeof(game.scoreText), "Score: %lu", game.score);
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
        game->score += calculateScore(&game->board) * game->streakCount;
    } else {
        game->streakCount = 0; // if no clearing reset back to 0
    }

    buildScoreText();
}