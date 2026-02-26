/**
    @file board.c (core)
    @author Fshimi Hawlk
    @date 2026-01-07
    @brief Board clearing logic implementation.
*/

#include "core/board.h"

#include "utils/globals.h"
#include "utils/utils.h"

bool isInBound(const s8Vector2 pos, const Board_St* const board) {
    return (0 <= pos.x) && (pos.x < board->width)
        && (0 <= pos.y) && (pos.y < board->height);
}

/**
    @brief Checks if a specific row on the board is completely filled.

    A row is full if every block in it has hitsLeft > 0.

    @param board Pointer to the constant board structure.
    @param row   Row index to check (0 to BOARD_HEIGHT-1).
    @return true if the row is full, false otherwise.
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
    @brief Checks if a specific column on the board is completely filled.

    A column is full if every block in it has hitsLeft > 0.

    @param board Pointer to the constant board structure.
    @param col   Column index to check (0 to BOARD_WIDTH-1).
    @return true if the column is full, false otherwise.
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
    @brief Clears all blocks in a specific row by setting hitsLeft to 0.

    @param board Pointer to the board structure.
    @param row   Row index to clear.
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
    @brief Clears all blocks in a specific column by setting hitsLeft to 0.

    @param board Pointer to the board structure.
    @param col   Column index to clear.
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

void updateBoardClearing(Board_St* const board) {
    if (board == NULL) {
        log_warn("Received NULL pointer");
        return;
    }
    
    for (u8 row = 0; row < board->height; ++row) {
        board->rowsToClear[row] = isRowFull(board, row);
    }

    for (u8 col = 0; col < board->width; ++col) {
        board->columnsToClear[col] = isColumnFull(board, col);
    }
}

bool checkBoardForClearing(const Board_St* const board) {
    if (board == NULL) {
        log_warn("Received NULL pointer");
        return false;
    }
    
    bool needClearing = false;
    for (u8 row = 0; row < board->height; ++row) {
        needClearing |= board->rowsToClear[row];
    }

    for (u8 col = 0; col < board->width; ++col) {
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

void printEmptyCells(const AnchorVec_St cells) {
    bool grid[8][8] = {false};
    for (u8 i = 0; i < cells.count; ++i) {
        grid[cells.items[i].y][cells.items[i].x] = true;
    }

    Color c = BLACK;

    log_debug("Empty cells:");
    for (u8 y = 0; y < 8; ++y) {
        for (u8 x = 0; x < 8; ++x) {
            if (grid[y][x]) {
                printf("\033[38;2;%u;%u;%um██\033[0m", c.r, c.g, c.b);
            } else {
                printf("  ");
            }
        }
        nl
    }
}

void printBoard(const Board_St* const board) {
    for (u8 r = 0; r < board->height; ++r) {
        for (u8 c = 0; c < board->width; ++c) {
            Block_St block = board->blocks[r][c];
            if (block.hitsLeft >= 0) {
                Color color = block.hitsLeft > 0 ? blockColors[block.colorIndex] : BLACK;
                printf("\033[38;2;%u;%u;%um██\033[0m", color.r, color.g, color.b);
            } else printf("  ");
        }
        nl
    }
}