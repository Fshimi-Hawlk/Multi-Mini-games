#include "ui/game.h"

void drawInformations(int score, int level, int lineNbTotal, int highScore) {
    char scoreText[30];
    char levelText[30];
    char linesText[30];
    char highScoreText[30];

    sprintf(scoreText, "Score: %d", score);
    sprintf(levelText, "Level: %d", level);
    sprintf(linesText, "Lines: %d", lineNbTotal);
    sprintf(highScoreText, "High score: %d", highScore);

    DrawText(scoreText, WINDOW_WIDTH / 40, WINDOW_HEIGHT / 2, 20, WHITE);
    DrawText(levelText, WINDOW_WIDTH / 40, WINDOW_HEIGHT / 2 + 30, 20, WHITE);
    DrawText(linesText, WINDOW_WIDTH / 40, WINDOW_HEIGHT / 2 + 60, 20, WHITE);
    DrawText(highScoreText, WINDOW_WIDTH / 40, WINDOW_HEIGHT / 2 + 120, 20, WHITE);
}

void drawPreview(board_t board, boardShape_st boardShape) {
    int offsetX = (WINDOW_WIDTH - (CELL_SIZE * BOARD_WIDTH)) / 2;
    int offsetY = (WINDOW_HEIGHT - (CELL_SIZE * BOARD_HEIGHT)) / 2;
    int x, y;

    for (; !isColliding(board, boardShape); (boardShape.position.y)++);
    (boardShape.position.y)--;

    for (int i = 0; i < 4; i++) {
        x = boardShape.shape[i].x + boardShape.position.x;
        y = boardShape.shape[i].y + boardShape.position.y;

        if (y < 0 || y >= BOARD_HEIGHT || x < 0 || x >= BOARD_WIDTH) continue;

        DrawRectangle((offsetX + x * CELL_SIZE) - 1, (offsetY + y * CELL_SIZE) - 1, CELL_SIZE + 2, CELL_SIZE + 2, WHITE);
        DrawRectangle(offsetX + x * CELL_SIZE, offsetY + y * CELL_SIZE, CELL_SIZE, CELL_SIZE, board[y][x]);
    }
}