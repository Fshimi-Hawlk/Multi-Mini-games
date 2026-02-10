/**
 * @file test_streak.c
 * @brief Unit tests for streak tracking and display.
 */

#include "core/game/game.h"
#include "core/game/board.h"

#include <assert.h>

static void initSizeWeights(GameState_St* const game) {
    f32 baseWeights[MAX_SHAPE_SIZE] = {
        [0] = 0.05f,    // size 1
        [1] = 0.20f,
        [2] = 0.25f,
        [3] = 0.25f,
        [4] = 0.10f,
        [5] = 0.05f,
        [6] = 0.00f,    // will never change, since no shape of size 7 exist, it's set by default to zero
        [7] = 0.00f,    // will never change, since no shape of size 8 exist, it's set by default to zero
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
    assert(testGame.streakCount == 0);
    log_info("OK");

    // Place without clearing: streak remains 0
    assert(checkBoardForClearing(&testGame.board) == false);
    log_info("OK");

    manageScore(&testGame, 1);
    assert(testGame.streakCount == 0);
    log_info("OK");

    // Simulate clearing (streak increases)
    for (u8 i = 0; i < 8; ++i) {
        testGame.board.blocks[0][i].hitsLeft = 1;
    }
    
    assert(checkBoardForClearing(&testGame.board) == true);
    log_info("OK");

    manageScore(&testGame, 8);
    assert(testGame.streakCount == 1);
    log_info("OK");

    // Another clear: streak to 2
    for (u8 i = 0; i < 8; ++i) {
        testGame.board.blocks[1][i].hitsLeft = 1;
    }
    
    assert(checkBoardForClearing(&testGame.board) == true);
    log_info("OK");

    manageScore(&testGame, 8);
    assert(testGame.streakCount == 2);
    log_info("OK");

    // Placement without clear: streak resets after threshold
    assert(checkBoardForClearing(&testGame.board) == false);
    log_info("OK");

    manageScore(&testGame, 1);
    assert(testGame.streakCount == 0);
    log_info("OK");
}

static void test_streak_text_formatting(void) {
    GameState_St testGame = {0};
    testGame.streakCount = 5;
    buildScoreRelatedTexts(&testGame);
    assert(strcmp(testGame.streakText, "Streak: 5") == 0);
    log_info("OK");

    testGame.streakCount = 0;
    buildScoreRelatedTexts(&testGame);
    assert(strcmp(testGame.streakText, "Streak: 0") == 0);
    log_info("OK");
}

int main(void) {
    test_streak_increment_and_reset();
    test_streak_text_formatting();
    log_info("Streak tests passed");
    return 0;
}

#define LOGGER_IMPLEMENTATION
#include "logger.h"

#define CONTEXT_ARENA_IMPLEMENTATION
#include "contextArena.h"

#define RAND_IMPLEMENTATION
#include "rand.h"