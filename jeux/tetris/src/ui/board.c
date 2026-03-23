#include "ui/board.h"

void drawBoard(board_t board) {
    int offsetX = (WINDOW_WIDTH - (CELL_SIZE * BOARD_WIDTH)) / 2;
    int offsetY = (WINDOW_HEIGHT - (CELL_SIZE * BOARD_HEIGHT)) / 2; 

    for (int y = 0; y < BOARD_HEIGHT; y++) {
        for (int x = 0; x < BOARD_WIDTH; x++) {
            DrawRectangleLines(offsetX + x * CELL_SIZE, offsetY + y * CELL_SIZE, CELL_SIZE, CELL_SIZE, BOARD_GRID_COLOR);
            DrawRectangle     (offsetX + x * CELL_SIZE, offsetY + y * CELL_SIZE, CELL_SIZE-2, CELL_SIZE-2, board[y][x]);
        }
    }
}
