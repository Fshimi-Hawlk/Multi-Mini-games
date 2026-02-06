#include "ui/shape.h"

void drawShape(boardShape_st boardShape) {
    int offsetX = (WINDOW_WIDTH - (CELL_SIZE * BOARD_WIDTH)) / 2;
    int offsetY = (WINDOW_HEIGHT - (CELL_SIZE * BOARD_HEIGHT)) / 2;
    int x, y;

    for (int i = 0; i < 4; i++) {
        x = boardShape.shape[i].x + boardShape.position.x;
        y = boardShape.shape[i].y + boardShape.position.y;

        if (y < 0)
            continue;

        DrawRectangle((offsetX + x * CELL_SIZE) - 1, (offsetY + y * CELL_SIZE) - 1, CELL_SIZE + 2, CELL_SIZE + 2, BOARD_GRID_COLOR);
        DrawRectangle(offsetX + x * CELL_SIZE, offsetY + y * CELL_SIZE, CELL_SIZE, CELL_SIZE, boardShape.color);
    }
}

void drawNextShape(boardShape_st boardShape) {
    int offsetX = WINDOW_WIDTH * (2.0 / 3);
    int offsetY = WINDOW_HEIGHT * (1.0 / 3);
    int x, y;

    DrawText("Next shape:", offsetX + 60, offsetY - 60, 20, WHITE);

    for (int i = 0; i < 4; i++) {
        x = boardShape.shape[i].x + boardShape.position.x;
        y = boardShape.shape[i].y + boardShape.position.y;

        DrawRectangle((offsetX + x * CELL_SIZE) - 1, (offsetY + y * CELL_SIZE) - 1, CELL_SIZE + 2, CELL_SIZE + 2, BOARD_GRID_COLOR);
        DrawRectangle(offsetX + x * CELL_SIZE, offsetY + y * CELL_SIZE, CELL_SIZE, CELL_SIZE, boardShape.color);
    }
}