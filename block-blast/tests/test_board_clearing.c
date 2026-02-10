/**
 * @file test_board_clearing.c
 * @brief Unit tests for row/column detection and clearing.
 */

#include "core/game/board.h"
#include "utils/common.h"

#include <assert.h>

/**
 * @brief Checks if a specific row on the board is completely filled.
 *
 * A row is full if every block in it has hitsLeft > 0.
 *
 * @param board Pointer to the constant board structure.
 * @param row   Row index to check (0 to BOARD_HEIGHT-1).
 * @return true if the row is full, false otherwise.
 */
static bool isRowFull(const Board_St* const board, const u8 row) {
    if (board == NULL) {
        log_warn("Received NULL pointer");
        return false;
    }
    
    for (u8 col = 0; col < board->width; ++col) {
        if (board->blocks[row][col].hitsLeft == 0) return false;
    }
    return true;
}

/**
 * @brief Checks if a specific column on the board is completely filled.
 *
 * A column is full if every block in it has hitsLeft > 0.
 *
 * @param board Pointer to the constant board structure.
 * @param col   Column index to check (0 to BOARD_WIDTH-1).
 * @return true if the column is full, false otherwise.
 */
static bool isColumnFull(const Board_St* const board, const u8 col) {
    if (board == NULL) {
        log_warn("Received NULL pointer");
        return false;
    }
    
    for (u8 row = 0; row < board->width; ++row) {
        if (board->blocks[row][col].hitsLeft == 0) return false;
    }
    return true;
}

/**
 * @brief Clears all blocks in a specific row by setting hitsLeft to 0.
 *
 * @param board Pointer to the board structure.
 * @param row   Row index to clear.
 */
static void clearRow(Board_St* const board, const u8 row) {
    if (board == NULL) {
        log_warn("Received NULL pointer");
        return;
    }
    
    for (u8 col = 0; col < board->width; ++col) {
        board->blocks[row][col].hitsLeft = 0;
    }
}

/**
 * @brief Clears all blocks in a specific column by setting hitsLeft to 0.
 *
 * @param board Pointer to the board structure.
 * @param col   Column index to clear.
 */
static void clearColumn(Board_St* const board, const u8 col) {
    if (board == NULL) {
        log_warn("Received NULL pointer");
        return;
    }

    for (u8 row = 0; row < board->width; ++row) {
        board->blocks[row][col].hitsLeft = 0;
    }
}

static void test_row_detection(void) {
    Board_St testBoard = {.width = 8, .height = 8};
    // Empty row: false
    assert(isRowFull(&testBoard, 0) == false);

    // Fill row
    for (u8 c = 0; c < 8; ++c) {
        testBoard.blocks[0][c].hitsLeft = 1;
    }
    assert(isRowFull(&testBoard, 0) == true);
}

static void test_check_for_clearing(void) {
    Board_St testBoard = {.width = 8, .height = 8};
    testBoard.rowsToClear = context_alloc(testBoard.height * sizeof(bool));
    testBoard.columnsToClear = context_alloc(testBoard.width * sizeof(bool));
    
    // No clears
    assert(checkBoardForClearing(&testBoard) == false);

    // Fill row 0 and col 0
    for (u8 i = 0; i < 8; ++i) {
        testBoard.blocks[0][i].hitsLeft = 1; // Row
        testBoard.blocks[i][0].hitsLeft = 1; // Col (overlaps at [0][0])
    }
    assert(checkBoardForClearing(&testBoard) == true);
    assert(testBoard.rowsToClear[0] == true);
    assert(testBoard.columnsToClear[0] == true);
}

static void test_clear_board(void) {
    Board_St testBoard = {.width = 8, .height = 8};
    testBoard.rowsToClear = context_alloc(testBoard.height * sizeof(bool));
    testBoard.columnsToClear = context_alloc(testBoard.width * sizeof(bool));
    
    // Setup clear
    for (u8 i = 0; i < 8; ++i) {
        testBoard.blocks[0][i].hitsLeft = 1;
    }
    
    assert(checkBoardForClearing(&testBoard) == true);
    log_info("OK");

    clearBoard(&testBoard);
    for (u8 i = 0; i < 8; ++i) {
        assert(testBoard.blocks[0][i].hitsLeft == 0); // Cleared
    }
}

static void test_empty_cell_count(void) {
    Board_St testBoard = {.width = 8, .height = 8};
    assert(getEmptyCellCount(&testBoard) == 64); // All empty

    testBoard.blocks[0][0].hitsLeft = 1;
    assert(getEmptyCellCount(&testBoard) == 63);
}

int main(void) {
    test_row_detection();
    test_check_for_clearing();
    test_clear_board();
    test_empty_cell_count();

    arena_free(&tempArena);
    arena_free(&globalArena);

    log_info("Board clearing tests passed");
    return 0;
}

#define LOGGER_IMPLEMENTATION
#include "logger.h"

#define CONTEXT_ARENA_IMPLEMENTATION
#include "contextArena.h"

#define RAND_IMPLEMENTATION
#include "rand.h"