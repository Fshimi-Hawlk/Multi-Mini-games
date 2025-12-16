#include "rendering/board2D.h"
#include "utils/globals.h"

void drawGameBoard(const Board_t board,
                   const s64Vector2_St flipped[], u64_t flippedCount,
                   s64Vector2_St lastMove) {
    // Draw cells
    for (int y = 0; y < BOARD_SIZE; ++y)
        for (int x = 0; x < BOARD_SIZE; ++x) {
            int px = x * CELL_PX_SIZE + boardOffset.x;
            int py = y * CELL_PX_SIZE + boardOffset.y;

            DrawRectangle(px, py, CELL_PX_SIZE, CELL_PX_SIZE, DARKGREEN);
            DrawRectangleLines(px, py, CELL_PX_SIZE, CELL_PX_SIZE, BLACK);

            if (board[y][x] == PLAYER)
                DrawCircle(px + CELL_PX_SIZE/2, py + CELL_PX_SIZE/2, CELL_PX_SIZE/2 - 6, WHITE);
            else if (board[y][x] == COMPUTER)
                DrawCircle(px + CELL_PX_SIZE/2, py + CELL_PX_SIZE/2, CELL_PX_SIZE/2 - 6, BLACK);
        }

    // Highlight last move + flipped pieces
    if (lastMove.x >= 0) {
        int lx = lastMove.x * CELL_PX_SIZE + boardOffset.x + CELL_PX_SIZE/2;
        int ly = lastMove.y * CELL_PX_SIZE + boardOffset.y + CELL_PX_SIZE/2;

        for (u64_t i = 0; i < flippedCount; ++i) {
            int fx = flipped[i].x * CELL_PX_SIZE + boardOffset.x + CELL_PX_SIZE/2;
            int fy = flipped[i].y * CELL_PX_SIZE + boardOffset.y + CELL_PX_SIZE/2;
            DrawCircle(fx, fy, CELL_PX_SIZE/2 - 3, ORANGE);
            DrawCircle(fx, fy, CELL_PX_SIZE/2 - 5, board[flipped[i].y][flipped[i].x] == PLAYER ? WHITE : BLACK);
        }

        DrawCircle(lx, ly, CELL_PX_SIZE/2 - 3, RED);
        DrawCircle(lx, ly, CELL_PX_SIZE/2 - 5, board[lastMove.y][lastMove.x] == PLAYER ? WHITE : BLACK);
    }

    // Coordinates
    for (int i = 0; i < BOARD_SIZE; ++i) {
        DrawText(TextFormat("%c", 'A' + i),
                 i * CELL_PX_SIZE + boardOffset.x + CELL_PX_SIZE/2 - 8,
                 boardOffset.y - 30, 20, BLACK);

        DrawText(TextFormat("%d", i + 1),
                 boardOffset.x - 25,
                 i * CELL_PX_SIZE + boardOffset.y + CELL_PX_SIZE/2 - 10, 20, BLACK);
    }
}