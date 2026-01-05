#include "utils/globals.h"
#include "core/game/board.h"

bool8 isInBound(const s8Vector2 pos) {
    return (0 <= pos.x) && (pos.x < game.board.width)
        && (0 <= pos.y) && (pos.y < game.board.height);
}


static bool8 isRowFull(const Board_St* const board, const u8 row) {
    for (u8 col = 0; col < board->width; ++col) {
        if (board->blocks[row][col].hitsLeft == 0) return false;
    }
    return true;
}

static bool8 isColumnFull(const Board_St* const board, const u8 col) {
    for (u8 row = 0; row < board->width; ++row) {
        if (board->blocks[row][col].hitsLeft == 0) return false;
    }
    return true;
}

static void clearRow(Board_St* const board, const u8 row) {
    for (u8 col = 0; col < board->width; ++col) {
        board->blocks[row][col].hitsLeft = 0;
    }
}

static void clearColumn(Board_St* const board, const u8 col) {
    for (u8 row = 0; row < board->width; ++row) {
        board->blocks[row][col].hitsLeft = 0;
    }
}

bool8 checkBoardForClearing(const Board_St* const board) {
    bool8 needClearing = false;
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
    for (u8 row = 0; row < board->height; ++row) {
        if (board->rowsToClear[row]) clearRow(board, row);
    }

    for (u8 col = 0; col < board->width; ++col) {
        if (board->columnsToClear[col]) clearColumn(board, col);
    }
}