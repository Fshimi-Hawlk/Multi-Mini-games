/**
 * @file board.c (core)
 * @author Fshimi Hawlk
 * @date 2026-01-07
 * @brief Board clearing logic implementation.
 */

#include "utils/userTypes.h"
#include "core/game/board.h"

bool isInBound(const s8Vector2 pos, const Board_St* const board) {
    return (0 <= pos.x) && (pos.x < board->width)
        && (0 <= pos.y) && (pos.y < board->height);
}

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

bool checkBoardForClearing(const Board_St* const board) {
    if (board == NULL) {
        log_warn("Received NULL pointer");
        return false;
    }
    
    bool needClearing = false;
    for (u8 row = 0; row < board->height; ++row) {
        board->rowsToClear[row] = isRowFull(board, row);
        needClearing |= board->rowsToClear[row];
    }

    for (u8 col = 0; col < board->width; ++col) {
        board->columnsToClear[col] = isColumnFull(board, col);
        needClearing |= board->columnsToClear[col];
    }

    return needClearing;
}

void clearBoard(Board_St* const board) {
    if (board == NULL) {
        log_warn("Received NULL pointer");
        return;
    }
    
    for (u8 row = 0; row < board->height; ++row) {
        if (board->rowsToClear[row]) clearRow(board, row);
    }

    for (u8 col = 0; col < board->width; ++col) {
        if (board->columnsToClear[col]) clearColumn(board, col);
    }
}

u32 getEmptyCellCount(const Board_St* const board) {
    if (board == NULL) {
        log_warn("Received NULL pointer");
        return 0;
    }

    u32 empty_cells = 0;
    for (u32 y = 0; y < board->height; y++) {
        for (u32 x = 0; x < board->width; x++) {
            if (board->blocks[y][x].hitsLeft == 0) {
                empty_cells++;
            }
        }
    }

    return empty_cells;
}