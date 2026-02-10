/**
 * @file test_board_grid.c
 * @brief Unit tests for 8x8 grid and board data.
 */

#include "core/game/board.h"
#include "setups/game.h"
#include "utils/userTypes.h"

#include <assert.h>

static void test_board_init(void) {
    GameState_St testGame = {0};
    initGame(&testGame); // Sets board
    assert(testGame.board.width == 8);
    log_info("OK");

    assert(testGame.board.height == 8);
    log_info("OK");

    for (u8 r = 0; r < 8; ++r) {
        for (u8 c = 0; c < 8; ++c) {
            assert(testGame.board.blocks[r][c].hitsLeft == 0);
        }
    }
    log_info("OK");
}

static void test_is_in_bound(void) {
    Board_St mockBoard = {
        .width = 8,
        .height = 8,
    };

    s8Vector2 pos = {3, 4};
    assert(isInBound(pos, &mockBoard) == true);
    log_info("OK");

    pos.x = -1;
    assert(isInBound(pos, &mockBoard) == false);
    log_info("OK");

    pos.x = 8; pos.y = 8;
    assert(isInBound(pos, &mockBoard) == false);
    log_info("OK");
}

static void test_empty_cell_count(void) {
    Board_St testBoard = {.width = 8, .height = 8};
    assert(getEmptyCellCount(&testBoard) == 64);
    log_info("OK");


    testBoard.blocks[0][0].hitsLeft = 1;
    testBoard.blocks[7][7].hitsLeft = -1; // Cleared not empty
    assert(getEmptyCellCount(&testBoard) == 62);
    log_info("OK");
}

int main(void) {
    test_board_init();
    test_is_in_bound();
    test_empty_cell_count();
    log_info("Board grid tests passed");
    return 0;
}

#define LOGGER_IMPLEMENTATION
#include "logger.h"

#define CONTEXT_ARENA_IMPLEMENTATION
#include "contextArena.h"

#define RAND_IMPLEMENTATION
#include "rand.h"