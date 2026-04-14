/**
    @file algo.c
    @author Léandre BAUDET
    @date 2026-04-14
    @date 2026-04-14
    @brief Implementation of AI algorithms for Tetris.
*/
#include "core/algo.h"
#include "core/shape.h"
#include "core/board.h"
#include "utils/utils.h"
#include <string.h>
#include <stdlib.h>

/**
    @brief Copies the content of one board to another.

    @param[in]  src   Source board to copy from.
    @param[out] dest  Destination board to copy to.
*/
void copyBoard(board_t src, board_t dest) {
    memcpy(dest, src, sizeof(board_t));
}

/**
    @brief Evaluates the current state of the board and returns a score.

    @param[in] board  The board to evaluate.
    @return           The calculated score for the board state.
*/
int evaluateBoard(board_t board) {
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
            if (!isColorsEqual(board[y][x], BOARD_BACKGROUND_COLOR)) {
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
            if (isColorsEqual(board[y][x], BOARD_BACKGROUND_COLOR)) {
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

/**
    @brief Simulates dropping a piece in a specific column.

    @param[in] board  The current board state.
    @param[in] piece  The piece to drop.
    @param[in] col    The column index to drop the piece in.
    @return           The resulting Y position of the piece after dropping.
*/
int simulateDrop(board_t board, boardShape_st piece, int col) {
    piece.position = (iVector2){ col, 0 };

    while (!isColliding(board, piece)) {
        piece.position.y++;
    }

    return --piece.position.y;
}

/**
    @brief Finds the best move for the current and next shape.

    @param[in] board      The current board state.
    @param[in] shape      The current shape to place.
    @param[in] nextShape   The next shape that will appear.
    @return               A struct containing the best position and rotation.
*/
moveAlgoResult_st findBestMove(board_t board, boardShape_st shape, boardShape_st nextShape) {
    board_t temp;
    int bestScore = -100000;
    moveAlgoResult_st result = { .position = {-1, -1}, .rotation = -1 };

    for (int rot = 0; rot < 4; ++rot) {
        for (int col = 0; col < BOARD_WIDTH; col++) {
            int row = simulateDrop(board, shape, col);
            if (row < 0) continue;
            iVector2 foundPosition = { col, row };
            if (isOOBAt(shape, foundPosition)) continue;

            for (int rotNext = 0; rotNext < 4; ++rotNext) {
                for (int colNext = 0; colNext < BOARD_WIDTH; colNext++) {
                    /* FIX: used `col` instead of `colNext` — the lookahead was simulating
                     * the next piece in the same column as the current piece. */
                    int rowNext = simulateDrop(board, nextShape, colNext);
                    if (rowNext < 0) continue;
                    iVector2 foundPositionNext = { colNext, rowNext };
                    if (isOOBAt(nextShape, foundPositionNext)) continue;

                    copyBoard(board, temp);
                    shape.position    = foundPosition;
                    nextShape.position = foundPositionNext;
                    putShapeInBoard(temp, shape);
                    putShapeInBoard(temp, nextShape);

                    int score = evaluateBoard(temp);
                    if (score > bestScore) {
                        bestScore        = score;
                        result.position  = foundPosition;
                        result.rotation  = rot;
                    }
                }
                rotationCW(&nextShape);
            }
        }

        rotationCW(&shape);
    }

    return result;
}
