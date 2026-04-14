/**
    @file board.c
    @author Léandre BAUDET
    @date 2026-04-14
    @date 2026-04-14
    @brief Implementation of board management for Tetris.
*/
#include "core/board.h"
#include "utils/utils.h"

/**
    @brief Initializes the board with the background color.

    @param[out] board  The board to initialize.
*/
void tetris_initBoard(board_t board) {
    for (int y = 0; y < BOARD_HEIGHT; y++)
        for (int x = 0; x < BOARD_WIDTH; x++)
            board[y][x] = BOARD_BACKGROUND_COLOR;
}

/**
    @brief Checks if the given coordinates are out of bounds.

    @param[in] x  X coordinate.
    @param[in] y  Y coordinate.
    @return       True if coordinates are out of bounds, false otherwise.
*/
bool areCoordinatesOOB(int x, int y) {
    return (y >= BOARD_HEIGHT) || (x < 0) || (x >= BOARD_WIDTH);
}

/**
    @brief Checks if the given position is out of bounds.

    @param[in] position  The position to check.
    @return              True if the position is out of bounds, false otherwise.
*/
bool isPositonOOB(iVector2 position) {
    return areCoordinatesOOB(position.x, position.y);
}

/**
    @brief Checks if a shape would be out of bounds at a specific position.

    @param[in] boardShape  The shape to check.
    @param[in] position    The position to check the shape at.
    @return                True if the shape is out of bounds at the position, false otherwise.
*/
bool isOOBAt(boardShape_st boardShape, iVector2 position) {
    for (int i = 0; i < 4; i++) {
        int x = boardShape.shape[i].x + position.x;
        int y = boardShape.shape[i].y + position.y;

        if (x < 0 || x >= BOARD_WIDTH) return true;
        if (y >= BOARD_HEIGHT) return true;
    }

    return false;
}

/**
    @brief Checks if a shape is currently out of bounds.

    @param[in] boardShape  The shape to check.
    @return                True if the shape is out of bounds, false otherwise.
*/
bool isOOB(boardShape_st boardShape) {
    return isOOBAt(boardShape, boardShape.position);
}

/**
    @brief Checks if a shape collides with the board at a specific position.

    @param[in] board       The current board state.
    @param[in] boardShape  The shape to check.
    @param[in] position    The position to check collision at.
    @return                True if there is a collision, false otherwise.
*/
bool isCollidingAt(board_t board, boardShape_st boardShape, iVector2 position) {
    for (int i = 0; i < 4; i++) {
        int y = boardShape.shape[i].y + position.y;
        int x = boardShape.shape[i].x + position.x;

        if (x < 0 || x >= BOARD_WIDTH)
            return true;

        if (y >= BOARD_HEIGHT)
            return true;

        if (y < 0)
            continue;

        if (!isColorsEqual(board[y][x], BOARD_BACKGROUND_COLOR))
            return true;
    }

    return false;
}

/**
    @brief Checks if a shape is currently colliding with the board.

    @param[in] board       The current board state.
    @param[in] boardShape  The shape to check.
    @return                True if there is a collision, false otherwise.
*/
bool isColliding(board_t board, boardShape_st boardShape) {
    return isCollidingAt(board, boardShape, boardShape.position);
}

/**
    @brief Places a shape into the board.

    @param[in,out] board       The board to modify.
    @param[in]     boardShape  The shape to place.
*/
void putShapeInBoard(board_t board, boardShape_st boardShape) {
    for (int i = 0; i < 4; i++) {
        int shapeCellY = boardShape.shape[i].y + boardShape.position.y;
        int shapeCellX = boardShape.shape[i].x + boardShape.position.x;

        if (shapeCellY >= 0 && shapeCellY < BOARD_HEIGHT && shapeCellX >= 0 && shapeCellX < BOARD_WIDTH) {
            board[shapeCellY][shapeCellX] = boardShape.color;
        }
    }
}

/**
    @brief Detects full lines in the board.

    @param[in]  board      The board to check.
    @param[out] lineArray  Array filled with the indices of full lines.
    @param[out] lineNb     Number of full lines detected.
*/
void detectFullLines(board_t board, int lineArray[4], int *lineNb) {
    int nInLign;
    *lineNb = 0;

    for (int y = 0; y < BOARD_HEIGHT; y++) {
        nInLign = 0;
        for (int x = 0; x < BOARD_WIDTH; x++) {
            if (!isColorsEqual(board[y][x], BOARD_BACKGROUND_COLOR))
                nInLign++;
        }
        if (nInLign == BOARD_WIDTH)
            lineArray[(*lineNb)++] = y;
    }
}

/**
    @brief Clears specific lines from the board.

    @param[in,out] board      The board to modify.
    @param[in]     lineArray  Array containing the indices of lines to clear.
    @param[in]     lineNb     Number of lines to clear.
*/
void clearLines(board_t board, int lineArray[4], int lineNb) {
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

/**
    @brief Handles the full process of detecting and clearing lines.

    @param[in,out] board      The board to modify.
    @param[out]    lineArray  Array filled with the indices of cleared lines.
    @param[out]    lineNb     Number of lines cleared.
*/
void handleLineClears(board_t board, int lineArray[4], int *lineNb) {
    detectFullLines(board, lineArray, lineNb);
    clearLines(board, lineArray, *lineNb);
}
