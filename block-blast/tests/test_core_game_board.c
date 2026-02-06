#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "utils/userTypes.h"

#include "core/game/board.h"
#include "core/game/board.c"  // To access static functions like isRowFull, etc.


int main(void) {
    printf("Running tests for core/game/board...\n");

    // Test isInBound
    {
        s8Vector2 pos = {0, 0};
        assert(isInBound(pos) == true && "isInBound should return true for (0,0)");

        pos.x = 7; pos.y = 7;  // Assuming BOARD_WIDTH/HEIGHT=8 from configs.h
        assert(isInBound(pos) == true && "isInBound should return true for (7,7)");

        pos.x = -1; pos.y = 0;
        assert(isInBound(pos) == false && "isInBound should return false for (-1,0)");

        pos.x = 8; pos.y = 0;
        assert(isInBound(pos) == false && "isInBound should return false for (8,0)");
    }

    // Test checkBoardForClearing and internal helpers (isRowFull, isColumnFull)
    {
        Board_St board = {0};
        board.width = 8;
        board.height = 8;

        // Empty board: no clearing
        assert(checkBoardForClearing(&board) == false && "Empty board should not need clearing");

        // Fill row 0 completely
        for (u8 col = 0; col < board.width; ++col) {
            board.blocks[0][col].hitsLeft = 1;
        }
        assert(isRowFull(&board, 0) == true && "Filled row should be full");
        assert(checkBoardForClearing(&board) == true && "Board with full row should need clearing");
        assert(board.rowsToClear[0] == true && "rowsToClear[0] should be set");

        // Fill column 0 completely
        memset(&board, 0, sizeof(board));  // Reset
        board.width = 8; board.height = 8;
        for (u8 row = 0; row < board.height; ++row) {
            board.blocks[row][0].hitsLeft = 1;
        }
        assert(isColumnFull(&board, 0) == true && "Filled column should be full");
        assert(checkBoardForClearing(&board) == true && "Board with full column should need clearing");
        assert(board.columnsToClear[0] == true && "columnsToClear[0] should be set");

        // Partial fill: no clearing
        memset(&board, 0, sizeof(board));
        board.width = 8; board.height = 8;
        for (u8 col = 0; col < board.width - 1; ++col) {
            board.blocks[0][col].hitsLeft = 1;
        }
        assert(isRowFull(&board, 0) == false && "Partial row should not be full");
        assert(checkBoardForClearing(&board) == false && "Partial fill should not need clearing");
    }

    // Test clearBoard and internal helpers (clearRow, clearColumn)
    {
        Board_St board = {0};
        board.width = 8;
        board.height = 8;

        // Fill row 0 and column 0
        for (u8 col = 0; col < board.width; ++col) board.blocks[0][col].hitsLeft = 1;
        for (u8 row = 0; row < board.height; ++row) board.blocks[row][0].hitsLeft = 1;  // Overlap at [0][0]

        checkBoardForClearing(&board);  // Sets rowsToClear and columnsToClear
        clearBoard(&board);

        // Verify row 0 cleared
        for (u8 col = 0; col < board.width; ++col) {
            assert(board.blocks[0][col].hitsLeft == 0 && "Row should be cleared");
        }

        // Verify column 0 cleared (row 0 already checked)
        for (u8 row = 1; row < board.height; ++row) {
            assert(board.blocks[row][0].hitsLeft == 0 && "Column should be cleared");
        }
    }

    printf("All tests passed for core/game/board!\n");
    return 0;
}