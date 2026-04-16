/**
    @file game.c
    @author Fshimi-Hawlk
    @date 2026-02-06
    @date 2026-04-14
    @brief Implementation of general UI elements for Tetris.
*/

#include "core/board.h"
#include "ui/game.h"

void tetrominoFall_drawInformations(int score, int level, int lineNbTotal, int highScore) {
    char scoreText[30];
    char levelText[30];
    char linesText[30];
    char highScoreText[30];

    snprintf(scoreText,     sizeof(scoreText),     "Score: %d",      score);
    snprintf(levelText,     sizeof(levelText),      "Level: %d",      level);
    snprintf(linesText,     sizeof(linesText),      "Lines: %d",      lineNbTotal);
    snprintf(highScoreText, sizeof(highScoreText),  "High score: %d", highScore);

    int sw = GetScreenWidth();
    int sh = GetScreenHeight();
    int offsetX = (sw - (CELL_SIZE * BOARD_WIDTH)) / 2;
    int offsetY = (sh - (CELL_SIZE * BOARD_HEIGHT)) / 2;

    DrawText(scoreText, offsetX - 150, offsetY + 50, 20, WHITE);
    DrawText(levelText, offsetX - 150, offsetY + 80, 20, WHITE);
    DrawText(linesText, offsetX - 150, offsetY + 110, 20, WHITE);
    
    DrawText("HIGH SCORE:", offsetX - 150, offsetY + 170, 15, GOLD);
    DrawText(highScoreText, offsetX - 150, offsetY + 190, 20, GOLD);
}

void tetrominoFall_drawPreview(Board_t board, BoardShape_St boardShape) {
    int offsetX = (GetScreenWidth()  - (CELL_SIZE * BOARD_WIDTH))  / 2;
    int offsetY = (GetScreenHeight() - (CELL_SIZE * BOARD_HEIGHT)) / 2;
    int x, y;

    for (; !tetrominoFall_isColliding(board, boardShape); (boardShape.position.y)++);
    (boardShape.position.y)--;

    for (int i = 0; i < 4; i++) {
        x = boardShape.shape[i].x + boardShape.position.x;
        y = boardShape.shape[i].y + boardShape.position.y;

        if (y < 0 || y >= BOARD_HEIGHT || x < 0 || x >= BOARD_WIDTH) continue;

        DrawRectangle     (offsetX + x * CELL_SIZE, offsetY + y * CELL_SIZE, CELL_SIZE - 2, CELL_SIZE - 2, board[y][x]);
        DrawRectangleLines(offsetX + x * CELL_SIZE, offsetY + y * CELL_SIZE, CELL_SIZE, CELL_SIZE, WHITE);
    }
}