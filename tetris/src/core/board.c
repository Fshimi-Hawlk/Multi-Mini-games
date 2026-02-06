#include "core/board.h"
#include "utils/utils.h"

void initBoard(board_t board) {
    for (int y = 0; y < BOARD_HEIGHT; y++)
        for (int x = 0; x < BOARD_WIDTH; x++)
            board[y][x] = BOARD_BACKGROUND_COLOR; 
}

bool areCoordinatesOOB(int x, int y) {
    return (y >= BOARD_HEIGHT) || (x < 0) || (x >= BOARD_WIDTH);
}

bool isPositonOOB(iVector2 position) {
    return areCoordinatesOOB(position.x, position.y);
}

bool isOOBAt(boardShape_st boardShape, iVector2 position) {
    for (int i = 0; i < 4; i++) {
        int x = boardShape.shape[i].x + position.x;
        int y = boardShape.shape[i].y + position.y;

        if (x < 0 || x >= BOARD_WIDTH) return true;
        if (y >= BOARD_HEIGHT) return true;
    }

    return false;
}

bool isOOB(boardShape_st boardShape) {
    return isOOBAt(boardShape, boardShape.position);
}

bool isCollidingAt(board_t board, boardShape_st boardShape, iVector2 position) {
    for (int i = 0; i < 4; i++) {
        int y = boardShape.shape[i].y + position.y;
        int x = boardShape.shape[i].x + position.x;

        // Collision si hors limites horizontales
        if (x < 0 || x >= BOARD_WIDTH)
            return true;
        
        // Collision si en bas du plateau
        if (y >= BOARD_HEIGHT)
            return true;
        
        // Collision si haut du plateau et on n'ignore pas
        if (y < 0)
            continue;

        // Collision si cellule occupée
        if (!isColorsEqual(board[y][x], BOARD_BACKGROUND_COLOR))
            return true;
    }

    return false;
}

bool isColliding(board_t board, boardShape_st boardShape) {
    return isCollidingAt(board, boardShape, boardShape.position);
}

void putShapeInBoard(board_t board, boardShape_st boardShape) {
    for (int i = 0; i < 4; i++) {
        int shapeCellY = boardShape.shape[i].y + boardShape.position.y;
        int shapeCellX = boardShape.shape[i].x + boardShape.position.x;

        // Vérifier les limites avant d'accéder
        if (shapeCellY >= 0 && shapeCellY < BOARD_HEIGHT && shapeCellX >= 0 && shapeCellX < BOARD_WIDTH) {
            board[shapeCellY][shapeCellX] = boardShape.color;
        }
    }
}

void detectFullLines(board_t board, int lineArray[4], int *lineNb) {
    int nInLign;
    *lineNb = 0;

    for (int y = 0; y < BOARD_HEIGHT; y++) {
        nInLign = 0;
        for (int x = 0; x < BOARD_WIDTH; x++) {
            if (memcmp(&board[y][x], &BOARD_BACKGROUND_COLOR, sizeof(BOARD_BACKGROUND_COLOR)))
                nInLign++;
        }
        if (nInLign == BOARD_WIDTH)
            lineArray[(*lineNb)++] = y;
    }
}

void clearLines(board_t board, int lineArray[4], int lineNb) {
    for (int i = 0; i < lineNb; i++) {
        for (int y = lineArray[i] - 1; y >= 0; y--) {
            for (int x = 0; x < BOARD_WIDTH; x++) {
                board[y + 1][x] = board[y][x];
            }
        }
        for (int x = 0; x < BOARD_WIDTH; x++) {
            board[0][x] = BOARD_BACKGROUND_COLOR;
        }
    }
}

void handleLineClears(board_t board, int lineArray[4], int *lineNb) {
    detectFullLines(board, lineArray, lineNb);
    clearLines(board, lineArray, *lineNb);
}