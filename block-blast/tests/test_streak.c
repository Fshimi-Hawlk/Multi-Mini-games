/**
 * @file test_streak.c
 * @brief Unit tests for streak tracking and display.
 */

#include "core/game/game.h"
#include "core/game/board.h"
#include "core/game/shape.h"
#include "utils/userTypes.h"

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

    // Mock a prefab for placement
    Prefab_St mockPrefab = {.blockCount = 1};
    Prefab_St linePrefab = {.blockCount = 8}; // Enough to fill a row

    ActivePrefab_St mockShape = {.prefab = &mockPrefab, .colorIndex = 0};
    ActivePrefab_St lineShape = {.prefab = &linePrefab, .colorIndex = 0};

    // Initial streak should be 0
    assert(testGame.streakCount == 0);

    // Place without clearing: streak remains 0
    placeShape(&mockShape, (u8Vector2) {0, 6}, &testGame.board);
    manageScore(&testGame, mockPrefab.blockCount);
    assert(testGame.streakCount == 0);

    // Simulate clearing (streak increases)
    bool originalClear = checkBoardForClearing(&testGame.board);
    assert(originalClear == false);

    placeShape(&lineShape, (u8Vector2) {0, 0}, &testGame.board);
    manageScore(&testGame, linePrefab.blockCount);
    clearBoard(&testGame.board);
    assert(testGame.streakCount == 1);

    // Another clear: streak to 2
    placeShape(&lineShape, (u8Vector2) {0, 1}, &testGame.board);
    manageScore(&testGame, linePrefab.blockCount);
    clearBoard(&testGame.board);
    assert(testGame.streakCount == 2);

    // Placement without clear: streak resets after threshold
    manageScore(&testGame, mockPrefab.blockCount);
    assert(testGame.streakCount == 0);
}

static void test_streak_text_formatting(void) {
    GameState_St testGame = {0};
    testGame.streakCount = 5;
    buildScoreRelatedTexts(&testGame);
    assert(strcmp(testGame.streakText, "Streak: 5") == 0);

    testGame.streakCount = 0;
    buildScoreRelatedTexts(&testGame);
    assert(strcmp(testGame.streakText, "Streak: 0") == 0);
}

static void test_adjust_weights_with_streak(void) {
    GameState_St testGame = {0};
    initSizeWeights(&testGame);

    // High score delta (good performance, from high streak clears) shifts to larger shapes
    adjustSizeWeights(&testGame, 1000.0f); // High delta
    assert(testGame.prefabManager.sizeWeights.runTimeWeights[4] > testGame.prefabManager.sizeWeights.baseWeights[4]); // Larger shapes boosted

    // Low score delta (poor performance, low streak) shifts to smaller
    adjustSizeWeights(&testGame, 10.0f); // Low delta
    assert(testGame.prefabManager.sizeWeights.runTimeWeights[1] > testGame.prefabManager.sizeWeights.baseWeights[1]); // Smaller boosted
}

int main(void) {
    test_streak_increment_and_reset();
    test_streak_text_formatting();
    test_adjust_weights_with_streak();
    log_info("Streak tests passed");
    return 0;
}

#define LOGGER_IMPLEMENTATION
#include "logger.h"

#define CONTEXT_ARENA_IMPLEMENTATION
#include "contextArena.h"

#define RAND_IMPLEMENTATION
#include "rand.h"