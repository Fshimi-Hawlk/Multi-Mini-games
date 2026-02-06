#include "core/algo.h"
#include "core/shape.h"
#include "core/board.h"
#include "utils/utils.h"

void copyBoard(board_t src, board_t dest) {
    for (int y = 0; y < BOARD_HEIGHT; y++) {
        for (int x = 0; x < BOARD_WIDTH; x++) {
            dest[y][x] = src[y][x];
        }
    }
}

int evaluateBoard(board_t board) {
    int holes = 0;
    int aggregateHeight = 0;
    int bumpiness = 0;
    int completeLines = 0;
    int maxHeight = 0;

    int columnHeights[BOARD_WIDTH];

    // 1. Hauteurs par colonne
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
            }
            else if (blockFound) {
                holes++; // trou sous un bloc
            }
        }
    }

    // 2. Rugosité
    for (int x = 0; x < BOARD_WIDTH - 1; x++) {
        bumpiness += abs(columnHeights[x] - columnHeights[x + 1]);
    }

    // 3. Lignes complètes
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

    // 4. Combinaison pondérée
    int score = 0;
    score += -10 * holes;
    score += -5 * aggregateHeight;
    score += -1 * bumpiness;
    score += -5 * maxHeight;
    score += 20 * completeLines;

    return score;
}

int simulateDrop(board_t board, boardShape_st piece, int col) {
    piece.position = (iVector2) { col, 0 };

    // Descente jusqu’à collision
    while (!isColliding(board, piece)) {
        piece.position.y++;
    }

    return --piece.position.y;
}

moveAlgoResult_st findBestMove(board_t board, boardShape_st shape, boardShape_st nextShape) {
    board_t temp;
    int bestScore = -100000;
    moveAlgoResult_st result = {.position = {-1, -1}, .rotation = -1};

    for (int rot = 0; rot < 4; ++rot) {
        for (int col = 0; col < BOARD_WIDTH; col++) {
            int row = simulateDrop(board, shape, col);
            if (row < 0) continue;
            iVector2 foundPosition = {col, row};
            if (isOOBAt(shape, foundPosition)) continue;
            
            for (int rotNext = 0; rotNext < 4; ++rotNext) {
                for (int colNext = 0; colNext < BOARD_WIDTH; colNext++) {
                    int rowNext = simulateDrop(board, nextShape, col);
                    if (rowNext < 0) continue;
                    iVector2 foundPositionNext = {colNext, rowNext};
                    if (isOOBAt(nextShape, foundPositionNext)) continue;
            
                    copyBoard(board, temp);
                    shape.position = foundPosition;
                    nextShape.position = foundPositionNext;
                    putShapeInBoard(temp, shape);
                    putShapeInBoard(temp, nextShape);

                    int score = evaluateBoard(temp);
                    if (score > bestScore) {
                        bestScore = score;
                        result.position = foundPosition;
                        result.rotation = rot;
                    }
                }
                rotationCW(&nextShape);
            }
        }

        rotationCW(&shape);
    }

    return result;
}