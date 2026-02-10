/**
 * @file test_scoring.c
 * @brief Unit tests for scoring system.
 */

#include "core/game/game.h"
#include "core/game/board.h"
#include "core/game/shape.h"
#include "utils/common.h"
#include "utils/globals.h"
#include "utils/userTypes.h"

#include <assert.h>

static void test_calculate_score(void) {
    Board_St testBoard = {.width = 8, .height = 8};
    testBoard.rowsToClear = context_alloc(testBoard.height * sizeof(bool));
    memset(testBoard.rowsToClear, 0, testBoard.height);
    testBoard.columnsToClear = context_alloc(testBoard.width * sizeof(bool));
    memset(testBoard.columnsToClear, 0, testBoard.width);

    // No lines: 0
    assert(calculateScore(&testBoard) == 0.0f);
    log_info("OK");

    // 1 line: 100
    testBoard.rowsToClear[0] = true;
    assert(calculateScore(&testBoard) == 100.0f);
    log_info("OK");

    // 2 lines: 100 * 2 * 1.5 = 300
    testBoard.rowsToClear[1] = true;
    assert(calculateScore(&testBoard) == 300.0f);
    log_info("OK");

    // Mixed row/col: 3 lines -> 100 * 3 * 2.0 = 600
    testBoard.columnsToClear[0] = true;
    assert(calculateScore(&testBoard) == 600.0f);
    log_info("OK");
}

static void test_manage_score(void) {
    GameState_St testGame = {0};
    testGame.board = (Board_St) {.width = 8, .height = 8};
    testGame.board.rowsToClear = context_alloc(testGame.board.height * sizeof(bool));
    testGame.board.columnsToClear = context_alloc(testGame.board.width * sizeof(bool));
    
    ActivePrefab_St mockShape = {.prefab = &prefabs[PREFAB_1x4]};

    // Base placement: +40 (no clear)
    placeShape(&mockShape, (u8Vector2) {0, 0}, &testGame.board);
    manageScore(&testGame, mockShape.prefab->blockCount);
    clearBoard(&testGame.board);
    assert(testGame.score == 40);
    log_info("OK");

    // With clear: +40 + calculated
    placeShape(&mockShape, (u8Vector2) {0, 4}, &testGame.board);
    manageScore(&testGame, mockShape.prefab->blockCount);
    clearBoard(&testGame.board);
    assert(testGame.score == 40 + 40 + 100);
    log_info("OK");
    assert(testGame.streakCount == 1);
    log_info("OK");
}

static void test_score_text(void) {
    GameState_St testGame = {0};
    testGame.score = 500;
    buildScoreRelatedTexts(&testGame);
    assert(strcmp(testGame.scoreText, "Score: 500") == 0);
    log_info("OK");
}

int main(void) {
    test_calculate_score();
    test_manage_score();
    test_score_text();

    arena_free(&tempArena);
    arena_free(&globalArena);

    log_info("Scoring tests passed");
    return 0;
}

#define LOGGER_IMPLEMENTATION
#include "logger.h"

#define CONTEXT_ARENA_IMPLEMENTATION
#include "contextArena.h"

#define RAND_IMPLEMENTATION
#include "rand.h"