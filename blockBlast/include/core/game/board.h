/**
 * @file board.h (core/game)
 * @author Fshimi Hawlk
 * @date 2026-01-07
 * @brief Board logic: bounds checking and line clearing.
 */

#ifndef CORE_GAME_BOARD_H
#define CORE_GAME_BOARD_H

#include "utils/userTypes.h"

/**
 * @brief Checks if a position is within the board bounds.
 *
 * @param pos The position to check.
 * @return true if in bounds, false otherwise.
 */
bool8 isInBound(const s8Vector2 pos);

/**
 * @brief Checks the board for full rows/columns that need clearing.
 *
 * @param board Pointer to the board.
 * @return true if clearing is needed, false otherwise.
 */
bool8 checkBoardForClearing(const Board_St* const board);

/**
 * @brief Clears marked rows and columns on the board.
 *
 * @param board Pointer to the board.
 */
void clearBoard(Board_St* const board);

#endif // CORE_GAME_BOARD_H