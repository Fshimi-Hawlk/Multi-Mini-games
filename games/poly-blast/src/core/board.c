/**
    @file board.c
    @author Kimi BERGE
    @date 2026-01-07
    @date 2026-04-14
    @brief Board clearing logic implementation.
*/
#include "core/board.h"

#include "utils/globals.h"

#include "sharedUtils/debug.h"

/**
    @brief Checks if a position is within the board bounds.

    @param[in]     pos          The position to check.
    @param[in]     board        Pointer to the board.
    @return                     true if in bounds, false otherwise.
*/
bool polyBlast_isInBound(const s8Vector2 pos, const Board_St* const board) {
    return (0 <= pos.x) && (pos.x < board->width)
        && (0 <= pos.y) && (pos.y < board->height);
}

/**
    @brief Checks if a specific row on the board is completely filled.

    @param[in]     board        Pointer to the board.
    @param[in]     row          Row index to check.
    @return                     true if the row is full, false otherwise.
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

    @param[in]     board        Pointer to the board.
    @param[in]     col          Column index to check.
    @return                     true if the column is full, false otherwise.
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
    @brief Clears all blocks in a specific row.

    @param[in,out] board        Pointer to the board.
    @param[in]     row          Row index to clear.
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
    @brief Clears all blocks in a specific column.

    @param[in,out] board        Pointer to the board.
    @param[in]     col          Column index to clear.
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

/**
    @brief Scans the board and marks full rows and columns for clearing.

    @param[in,out] board        Pointer to the board.
*/
void polyBlast_updateBoardClearing(Board_St* const board) {
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

/**
    @brief Scans the board's rows/cols to clear arrays.

    @param[in]     board        Pointer to the board.
    @return                     true if at least one row or column is full, false otherwise.
*/
bool polyBlast_checkBoardForClearing(const Board_St* const board) {
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

/**
    @brief Clears marked rows and columns on the board.

    @param[in,out] board        Pointer to the board.
*/
void polyBlast_clearBoard(Board_St* const board) {
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

/**
    @brief Counts empty cells (hitsLeft == 0) on the board.

    @param[in]     board        Pointer to the board.
    @return                     Amount of empty cells.
*/
u32 polyBlast_getEmptyCellCount(const Board_St* const board) {
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

/**
    @brief Prints the positions of empty cells to the debug log.

    @param[in]     cells        Vector of anchor positions.
*/
void polyBlast_printEmptyCells(const AnchorVec_St cells) {
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

/**
    @brief Prints the current state of the board to the console.

    @param[in]     board        Pointer to the board.
*/
void polyBlast_printBoard(const Board_St* const board) {
    for (u8 r = 0; r < board->height; ++r) {
        for (u8 c = 0; c < board->width; ++c) {
            Block_St block = board->blocks[r][c];
            if (block.hitsLeft >= 0) {
                Color color = block.hitsLeft > 0 ? polyBlast_blockColors[block.colorIndex] : BLACK;
                printf("\033[38;2;%u;%u;%um██\033[0m", color.r, color.g, color.b);
            } else printf("  ");
        }
        nl
    }
}