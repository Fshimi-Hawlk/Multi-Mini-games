/**
    @file shape.c
    @author Fshimi-Hawlk
    @date 2026-02-06
    @date 2026-04-14
    @brief Implementation of shape rendering UI for Tetris.
*/

#include "ui/shape.h"

void tetrominoFall_drawShape(BoardShape_St boardShape) {
    int offsetX = (GetScreenWidth()  - (CELL_SIZE * BOARD_WIDTH))  / 2;
    int offsetY = (GetScreenHeight() - (CELL_SIZE * BOARD_HEIGHT)) / 2;
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

void tetrominoFall_drawNextShape(BoardShape_St boardShape) {
    int sw = GetScreenWidth();
    int sh = GetScreenHeight();
    int boardOffsetX = (sw - (CELL_SIZE * BOARD_WIDTH)) / 2;
    int boardOffsetY = (sh - (CELL_SIZE * BOARD_HEIGHT)) / 2;

    int offsetX = boardOffsetX + (BOARD_WIDTH * CELL_SIZE) + 50;
    int offsetY = boardOffsetY + 50;
    int x, y;

    // Draw a box for the next shape
    DrawRectangleLines(offsetX - 10, offsetY - 10, CELL_SIZE * 5, CELL_SIZE * 5, WHITE);
    DrawText("NEXT", offsetX + 10, offsetY - 35, 20, GOLD);

    boardShape.position = (iVector2){0, 0};

    // Center adjustment for better look in the box
    int centerAdjX = (CELL_SIZE * 4 - 4 * CELL_SIZE) / 2 + 10;
    int centerAdjY = 10;

    for (int i = 0; i < 4; i++) {
        x = boardShape.shape[i].x;
        y = boardShape.shape[i].y;

        DrawRectangle     (offsetX + centerAdjX + x * CELL_SIZE, offsetY + centerAdjY + y * CELL_SIZE, CELL_SIZE - 2, CELL_SIZE - 2, boardShape.color);
        DrawRectangleLines(offsetX + centerAdjX + x * CELL_SIZE, offsetY + centerAdjY + y * CELL_SIZE, CELL_SIZE, CELL_SIZE, BOARD_GRID_COLOR);
    }
}