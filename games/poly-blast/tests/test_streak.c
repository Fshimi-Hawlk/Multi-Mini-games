/**
    @file test_streak.c
    @author Fshimi-Hawlk
    @date 2026-04-10
    @brief Unit tests for streak tracking and display.

    Contributors:
        - Fshimi-Hawlk:
            - Updated to new ScoringState_St and manageScoreAndStreak()
            - Fixed arena usage and header includes
*/

#include "core/game.h"
#include "core/board.h"

#include <assert.h>

static void initSizeWeights(GameState_St* const game) {
    f32 baseWeights[MAX_SHAPE_SIZE] = {
        [0] = 0.05f,    // size 1
        [1] = 0.20f,
        [2] = 0.25f,
        [3] = 0.25f,
        [4] = 0.10f,
        [5] = 0.05f,
        [6] = 0.00f,
        [7] = 0.00f,
        [8] = 0.10f     // size 9
    };

    for (u8 i = 0; i < MAX_SHAPE_SIZE; i++) {
        game->prefabManager.sizeWeights.runTimeWeights[i] = game->prefabManager.sizeWeights.baseWeights[i] = baseWeights[i];
    }
}

static void test_streak_increment_and_reset(void) {
    GameState_St testGame = {0};
    testGame.board.width = testGame.board.height = 8;
    testGame.board.rowsToClear = context_alloc(testGame.board.height * sizeof(bool));
    testGame.board.columnsToClear = context_alloc(testGame.board.width * sizeof(bool));

    initSizeWeights(&testGame);

    // Initial streak should be 0
    assert(testGame.scoring.streakCount == 0);
    log_info("OK");

    updateBoardClearing(&testGame.board);

    // Place without clearing: streak remains 0
    assert(checkBoardForClearing(&testGame.board) == false);
    log_info("OK");

    manageScoreAndStreak(&testGame.scoring, &testGame.board, 1);
    assert(testGame.scoring.streakCount == 0);
    log_info("OK");

    // Simulate clearing (streak increases)
    for (u8 i = 0; i < 8; ++i) {
        testGame.board.blocks[0][i].hitsLeft = 1;
    }

    updateBoardClearing(&testGame.board);

    assert(checkBoardForClearing(&testGame.board) == true);
    log_info("OK");

    manageScoreAndStreak(&testGame.scoring, &testGame.board, 8);
    assert(testGame.scoring.streakCount == 1);
    log_info("OK");

    // Another clear: streak to 2
    for (u8 i = 0; i < 8; ++i) {
        testGame.board.blocks[1][i].hitsLeft = 1;
    }

    updateBoardClearing(&testGame.board);

    assert(checkBoardForClearing(&testGame.board) == true);
    log_info("OK");

    manageScoreAndStreak(&testGame.scoring, &testGame.board, 8);
    assert(testGame.scoring.streakCount == 2);
    log_info("OK");

    clearBoard(&testGame.board);
    updateBoardClearing(&testGame.board);

    // Placement without clear: streak resets after threshold
    assert(checkBoardForClearing(&testGame.board) == false);
    log_info("OK");

    manageScoreAndStreak(&testGame.scoring, &testGame.board, 1);
    assert(testGame.scoring.streakCount == 0);
    log_info("OK");
}

static void test_streak_text_formatting(void) {
    GameState_St testGame = {0};
    testGame.scoring.streakCount = 5;
    buildScoreRelatedTexts(&testGame.scoring);
    assert(strcmp(testGame.scoring.streakText, "Streak: 5") == 0);
    log_info("OK");

    testGame.scoring.streakCount = 0;
    buildScoreRelatedTexts(&testGame.scoring);
    assert(strcmp(testGame.scoring.streakText, "Streak: 0") == 0);
    log_info("OK");
}

int main(void) {
    test_streak_increment_and_reset();
    test_streak_text_formatting();
    log_info("Streak tests passed");
    return 0;
}