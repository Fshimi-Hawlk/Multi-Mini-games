#include "ui/shape.h"

void tetris_drawShape(boardShape_st boardShape) {
    int offsetX = (WINDOW_WIDTH - (CELL_SIZE * BOARD_WIDTH)) / 2;
    int offsetY = (WINDOW_HEIGHT - (CELL_SIZE * BOARD_HEIGHT)) / 2;
    int x, y;

    for (int i = 0; i < 4; i++) {
        x = boardShape.shape[i].x + boardShape.position.x;
        y = boardShape.shape[i].y + boardShape.position.y;

        if (y < 0)
            continue;

        DrawRectangle     (offsetX + x * CELL_SIZE, offsetY + y * CELL_SIZE, CELL_SIZE - 2, CELL_SIZE - 2, boardShape.color);
        DrawRectangleLines(offsetX + x * CELL_SIZE, offsetY + y * CELL_SIZE, CELL_SIZE, CELL_SIZE, BOARD_GRID_COLOR);
    }
}

void drawNextShape(boardShape_st boardShape) {
    int offsetX = WINDOW_WIDTH * (2.0 / 3);
    int offsetY = WINDOW_HEIGHT * (1.0 / 3);
    int x, y;

    DrawText("Next shape:", offsetX + 60, offsetY - 60, 20, WHITE);

    /* FIX: boardShape.position is {4, 0} (the spawn column) from randomShape().
     * Drawing at offsetX + (shape[i].x + 4) * CELL_SIZE pushed I-piece cells
     * (x up to 6) to pixel 266+150=416, outside WINDOW_WIDTH=400.
     * Reset position to {0, 0} so cells render centered on offsetX. */
    boardShape.position = (iVector2){0, 0};

    for (int i = 0; i < 4; i++) {
        x = boardShape.shape[i].x + boardShape.position.x;
        y = boardShape.shape[i].y + boardShape.position.y;

        DrawRectangle     (offsetX + x * CELL_SIZE, offsetY + y * CELL_SIZE, CELL_SIZE - 2, CELL_SIZE - 2, boardShape.color);
        DrawRectangleLines(offsetX + x * CELL_SIZE, offsetY + y * CELL_SIZE, CELL_SIZE, CELL_SIZE, BOARD_GRID_COLOR);
    }
}