/**
    @file board.h
    @author Fshimi-Hawlk
    @date 2026-02-06
    @date 2026-04-14
    @brief Board management functions for Tetris.
*/
#ifndef CORE_BOARD_H
#define CORE_BOARD_H

#include "utils/types.h"

/**
    @brief Initializes the board with the background color.

    @param[out] board  The board to initialize.
*/
void tetris_initBoard(board_t board);

/**
    @brief Checks if the given coordinates are out of bounds.

    @param[in] x  X coordinate.
    @param[in] y  Y coordinate.
    @return       True if coordinates are out of bounds, false otherwise.
*/
bool areCoordinatesOOB(int x, int y);

/**
    @brief Checks if the given position is out of bounds.

    @param[in] position  The position to check.
    @return              True if the position is out of bounds, false otherwise.
*/
bool isPositonOOB(iVector2 position);

/**
    @brief Checks if a shape would be out of bounds at a specific position.

    @param[in] boardShape  The shape to check.
    @param[in] position    The position to check the shape at.
    @return                True if the shape is out of bounds at the position, false otherwise.
*/
bool isOOBAt(boardShape_st boardShape, iVector2 position);

/**
    @brief Checks if a shape is currently out of bounds.

    @param[in] boardShape  The shape to check.
    @return                True if the shape is out of bounds, false otherwise.
*/
bool isOOB(boardShape_st boardShape);

/**
    @brief Checks if a shape collides with the board at a specific position.

    @param[in] board       The current board state.
    @param[in] boardShape  The shape to check.
    @param[in] position    The position to check collision at.
    @return                True if there is a collision, false otherwise.
*/
bool isCollidingAt(board_t board, boardShape_st boardShape, iVector2 position);

/**
    @brief Checks if a shape is currently colliding with the board.

    @param[in] board       The current board state.
    @param[in] boardShape  The shape to check.
    @return                True if there is a collision, false otherwise.
*/
bool isColliding(board_t board, boardShape_st boardShape);

/**
    @brief Places a shape into the board.

    @param[in,out] board       The board to modify.
    @param[in]     boardShape  The shape to place.
*/
void putShapeInBoard(board_t board, boardShape_st boardShape);

/**
    @brief Detects full lines in the board.

    @param[in]  board      The board to check.
    @param[out] lineArray  Array filled with the indices of full lines.
    @param[out] lineNb     Number of full lines detected.
*/
void detectFullLines(board_t board, int lineArray[4], int *lineNb);

/**
    @brief Clears specific lines from the board.

    @param[in,out] board      The board to modify.
    @param[in]     lineArray  Array containing the indices of lines to clear.
    @param[in]     lineNb     Number of lines to clear.
*/
void clearLines(board_t board, int lineArray[4], int lineNb);

/**
    @brief Handles the full process of detecting and clearing lines.

    @param[in,out] board      The board to modify.
    @param[out]    lineArray  Array filled with the indices of cleared lines.
    @param[out]    lineNb     Number of lines cleared.
*/
void handleLineClears(board_t board, int lineArray[4], int *lineNb);

#endif // CORE_BOARD_H
