/**
    @file test_game_over.c
    @author Fshimi-Hawlk
    @date 2026-04-11
    @brief Unit tests for game-over detection and canPlaceAll permutation search.

    Tests the core logic that decides whether the current three shapes can still
    be legally placed on the board in any order.
*/

#include "core/game.h"
#include "core/placement.h"
#include "core/board.h"
#include "setups/game.h"
#include "setups/shape.h"

#include "utils/globals.h"

#include <assert.h>

/* ------------------------------------------------------------------ */
/* Helper to quickly fill the entire board (used to force game over)  */
/* ------------------------------------------------------------------ */
static void fillBoardCompletely(Board_St* const board) {
    for (u8 r = 0; r < board->height; ++r) {
        for (u8 c = 0; c < board->width; ++c) {
            board->blocks[r][c].hitsLeft = 1;
        }
    }
    updateBoardClearing(board);
}

/* ------------------------------------------------------------------ */
/* Test: canPlaceAll returns true when there is at least one valid    */
/*       permutation that places all three shapes.                    */
/* ------------------------------------------------------------------ */
static void test_can_place_all_success(void) {
    Board_St testBoard = {.width = 8, .height = 8};
    initBoard(&testBoard);   // zeros everything + allocates clearing arrays

    // Three very small non-overlapping shapes that easily fit
    Shape_St shapes[3] = {
        {.prefab = &prefabs[PREFAB_1x1], .placed = false},
        {.prefab = &prefabs[PREFAB_1x1], .placed = false},
        {.prefab = &prefabs[PREFAB_1x2], .placed = false}
    };

    static const u8 order[3] = {0, 1, 2};

    bool result = canPlaceAll(&testBoard, shapes, order, 0);

    assert(result == true);
    log_info("OK - canPlaceAll succeeded with easy shapes");
}

/* ------------------------------------------------------------------ */
/* Test: canPlaceAll returns false when no permutation works          */
/*       (board is completely full).                                  */
/* ------------------------------------------------------------------ */
static void test_can_place_all_failure_full_board(void) {
    Board_St testBoard = {.width = 8, .height = 8};
    initBoard(&testBoard);
    fillBoardCompletely(&testBoard);

    Shape_St shapes[3] = {
        {.prefab = &prefabs[PREFAB_1x1], .placed = false},
        {.prefab = &prefabs[PREFAB_1x1], .placed = false},
        {.prefab = &prefabs[PREFAB_1x2], .placed = false}
    };

    static const u8 order[3] = {0, 1, 2};

    bool result = canPlaceAll(&testBoard, shapes, order, 0);

    assert(result == false);
    log_info("OK - canPlaceAll correctly returned false on full board");
}

/* ------------------------------------------------------------------ */
/* Test: testGameOver correctly detects game over using all 6 perms   */
/* ------------------------------------------------------------------ */
static void test_test_game_over(void) {
    GameState_St testGame = {0};
    testGame.board.width = testGame.board.height = 8;
    initBoard(&testGame.board);

    // Case 1: Should NOT be game over (plenty of space)
    Shape_St easySlots[3] = {
        {.prefab = &prefabs[PREFAB_1x1]},
        {.prefab = &prefabs[PREFAB_1x2]},
        {.prefab = &prefabs[PREFAB_L3]}
    };

    bool isOver = testGameOver(testGame.board, easySlots);
    assert(isOver == false);
    log_info("OK - testGameOver correctly returned false with space");

    // Case 2: Should be game over (board full)
    fillBoardCompletely(&testGame.board);

    isOver = testGameOver(testGame.board, easySlots);
    assert(isOver == true);
    log_info("OK - testGameOver correctly detected game over on full board");
}

/* ------------------------------------------------------------------ */
/* Test: canPlaceAll respects already-placed shapes (skip them)       */
/* ------------------------------------------------------------------ */
static void test_can_place_all_skips_placed_shapes(void) {
    Board_St testBoard = {.width = 8, .height = 8};
    initBoard(&testBoard);

    Shape_St shapes[3] = {
        {.prefab = &prefabs[PREFAB_1x1], .placed = true},   // already placed
        {.prefab = &prefabs[PREFAB_1x1], .placed = false},
        {.prefab = &prefabs[PREFAB_1x1], .placed = false}
    };

    static const u8 order[3] = {0, 1, 2};

    // Even with one placed shape, the other two must still fit
    bool result = canPlaceAll(&testBoard, shapes, order, 0);

    assert(result == true);
    log_info("OK - canPlaceAll correctly skipped already-placed shape");
}

int main(void) {
    SetTraceLogLevel(LOG_WARNING);

    // Make sure prefabs are initialized before any test
    initPrefabsAndVariants(&prefabsBag, GAME_PREFAB_VARIANT_DEFAULT);

    test_can_place_all_success();
    test_can_place_all_failure_full_board();
    test_test_game_over();
    test_can_place_all_skips_placed_shapes();

    arena_free(&tempArena);
    arena_free(&globalArena);

    log_info("Game-over and canPlaceAll tests passed");
    return 0;
}