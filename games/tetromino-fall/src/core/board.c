/**
    @file board.c
    @author Fshimi-Hawlk
    @date 2026-02-06
    @date 2026-04-14
    @brief Implementation of board management for Tetris.
*/

#include "core/board.h"

void tetrominoFall_initBoard(Board_t board) {
    for (int y = 0; y < BOARD_HEIGHT; y++)
        for (int x = 0; x < BOARD_WIDTH; x++)
            board[y][x] = BOARD_BACKGROUND_COLOR;
}

bool tetrominoFall_areCoordinatesOOB(int x, int y) {
    return (y >= BOARD_HEIGHT) || (x < 0) || (x >= BOARD_WIDTH);
}

bool tetrominoFall_isPositionOOB(iVector2 position) {
    return tetrominoFall_areCoordinatesOOB(position.x, position.y);
}

bool tetrominoFall_isOOBAt(BoardShape_St boardShape, iVector2 position) {
    for (int i = 0; i < 4; i++) {
        int x = boardShape.shape[i].x + position.x;
        int y = boardShape.shape[i].y + position.y;

        if (x < 0 || x >= BOARD_WIDTH) return true;
        if (y >= BOARD_HEIGHT) return true;
    }

    return false;
}

bool tetrominoFall_isOOB(BoardShape_St boardShape) {
    return tetrominoFall_isOOBAt(boardShape, boardShape.position);
}

bool tetrominoFall_isCollidingAt(Board_t board, BoardShape_St boardShape, iVector2 position) {
    for (int i = 0; i < 4; i++) {
        int y = boardShape.shape[i].y + position.y;
        int x = boardShape.shape[i].x + position.x;

        if (x < 0 || x >= BOARD_WIDTH)
            return true;

        if (y >= BOARD_HEIGHT)
            return true;

        if (y < 0)
            continue;

        if (!ColorIsEqual(board[y][x], BOARD_BACKGROUND_COLOR))
            return true;
    }

    return false;
}

bool tetrominoFall_isColliding(Board_t board, BoardShape_St boardShape) {
    return tetrominoFall_isCollidingAt(board, boardShape, boardShape.position);
}

void tetrominoFall_putShapeInBoard(Board_t board, BoardShape_St boardShape) {
    for (int i = 0; i < 4; i++) {
        int shapeCellY = boardShape.shape[i].y + boardShape.position.y;
        int shapeCellX = boardShape.shape[i].x + boardShape.position.x;

        if (shapeCellY >= 0 && shapeCellY < BOARD_HEIGHT && shapeCellX >= 0 && shapeCellX < BOARD_WIDTH) {
            board[shapeCellY][shapeCellX] = boardShape.color;
        }
    }
}

void tetrominoFall_detectFullLines(Board_t board, int lineArray[4], int *lineNb) {
    int nInLign;
    *lineNb = 0;

    for (int y = 0; y < BOARD_HEIGHT; y++) {
        nInLign = 0;
        for (int x = 0; x < BOARD_WIDTH; x++) {
            if (!ColorIsEqual(board[y][x], BOARD_BACKGROUND_COLOR))
                nInLign++;
        }
        if (nInLign == BOARD_WIDTH)
            lineArray[(*lineNb)++] = y;
    }
}

void tetrominoFall_clearLines(Board_t board, int lineArray[4], int lineNb) {
    for (int i = lineNb - 1; i >= 0; i--) {
        int line = lineArray[i];

        // Shift all lines above one cell down
        for (int y = line; y > 0; y--)
            for (int x = 0; x < BOARD_WIDTH; x++)
                board[y][x] = board[y - 1][x];

        // Clear the top line
        for (int x = 0; x < BOARD_WIDTH; x++)
            board[0][x] = BOARD_BACKGROUND_COLOR;

        // Lines above have dropped by one rank: adjust remaining indices
        for (int j = i - 1; j >= 0; j--)
            lineArray[j]++;
    }
}

void tetrominoFall_handleLineClears(Board_t board, int lineArray[4], int *lineNb) {
    tetrominoFall_detectFullLines(board, lineArray, lineNb);
    tetrominoFall_clearLines(board, lineArray, *lineNb);
}
