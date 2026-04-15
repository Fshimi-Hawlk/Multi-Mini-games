/**
    @file algo.c
    @author Fshimi-Hawlk
    @date 2026-02-06
    @date 2026-04-14
    @brief Implementation of AI algorithms for Tetris.
*/

#include "core/algo.h"
#include "core/shape.h"
#include "core/board.h"

void tetrominoFall_copyBoard(Board_t src, Board_t dest) {
    memcpy(dest, src, sizeof(Board_t));
}

int tetrominoFall_evaluateBoard(Board_t board) {
    int holes = 0;
    int aggregateHeight = 0;
    int bumpiness = 0;
    int completeLines = 0;
    int maxHeight = 0;

    int columnHeights[BOARD_WIDTH];

    // 1. Column heights
    for (int x = 0; x < BOARD_WIDTH; x++) {
        columnHeights[x] = 0;
        bool blockFound = false;
        for (int y = 0; y < BOARD_HEIGHT; y++) {
            if (!ColorIsEqual(board[y][x], BOARD_BACKGROUND_COLOR)) {
                if (!blockFound) {
                    blockFound = true;
                    columnHeights[x] = BOARD_HEIGHT - y;
                    aggregateHeight += columnHeights[x];
                    if (columnHeights[x] > maxHeight)
                        maxHeight = columnHeights[x];
                }
            } else if (blockFound) {
                holes++;
            }
        }
    }

    // 2. Bumpiness
    for (int x = 0; x < BOARD_WIDTH - 1; x++) {
        bumpiness += abs(columnHeights[x] - columnHeights[x + 1]);
    }

    // 3. Complete lines
    for (int y = 0; y < BOARD_HEIGHT; y++) {
        bool full = true;
        for (int x = 0; x < BOARD_WIDTH; x++) {
            if (ColorIsEqual(board[y][x], BOARD_BACKGROUND_COLOR)) {
                full = false;
                break;
            }
        }
        if (full)
            completeLines++;
    }

    // 4. Weighted combination
    int score = 0;
    score += -10 * holes;
    score += -5  * aggregateHeight;
    score += -1  * bumpiness;
    score += -5  * maxHeight;
    score += 20  * completeLines;

    return score;
}

int tetrominoFall_simulateDrop(Board_t board, BoardShape_St piece, int col) {
    piece.position = (iVector2){ col, 0 };

    while (!tetrominoFall_isColliding(board, piece)) {
        piece.position.y++;
    }

    return --piece.position.y;
}

MoveAlgoResult_St tetrominoFall_findBestMove(Board_t board, BoardShape_St shape, BoardShape_St nextShape) {
    Board_t temp;
    int bestScore = -100000;
    MoveAlgoResult_St result = { .position = {-1, -1}, .rotation = -1 };

    for (int rot = 0; rot < 4; ++rot) {
        for (int col = 0; col < BOARD_WIDTH; col++) {
            int row = tetrominoFall_simulateDrop(board, shape, col);
            if (row < 0) continue;
            iVector2 foundPosition = { col, row };
            if (tetrominoFall_isOOBAt(shape, foundPosition)) continue;

            for (int rotNext = 0; rotNext < 4; ++rotNext) {
                for (int colNext = 0; colNext < BOARD_WIDTH; colNext++) {
                    /* FIX: used `col` instead of `colNext` — the lookahead was simulating
                     * the next piece in the same column as the current piece. */
                    int rowNext = tetrominoFall_simulateDrop(board, nextShape, colNext);
                    if (rowNext < 0) continue;
                    iVector2 foundPositionNext = { colNext, rowNext };
                    if (tetrominoFall_isOOBAt(nextShape, foundPositionNext)) continue;

                    tetrominoFall_copyBoard(board, temp);
                    shape.position    = foundPosition;
                    nextShape.position = foundPositionNext;
                    tetrominoFall_putShapeInBoard(temp, shape);
                    tetrominoFall_putShapeInBoard(temp, nextShape);

                    int score = tetrominoFall_evaluateBoard(temp);
                    if (score > bestScore) {
                        bestScore        = score;
                        result.position  = foundPosition;
                        result.rotation  = rot;
                    }
                }

                tetrominoFall_rotationCW(&nextShape);
            }
        }

        tetrominoFall_rotationCW(&shape);
    }

    return result;
}
