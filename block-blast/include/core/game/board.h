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
 * @brief Scans the board and marks full rows and columns for clearing.
 *
 * A row/column is considered full when **every** cell has hitsLeft > 0.
 * Empty cells (hitsLeft == 0) disqualify the line.
 *
 * Results are written into board->rowsToClear[] and board->columnsToClear[].
 * These arrays **must** be valid and zero-initialized before calling this function.
 *
 * @return true if at least one row **or** column is full (i.e. clearBoard() would do something)
 * @return false if the board has no completable lines
 */
bool8 checkBoardForClearing(const Board_St* const board);

/**
 * @brief Clears marked rows and columns on the board.
 *
 * @param board Pointer to the board.
 */
void clearBoard(Board_St* const board);

/**
 * @brief Counts empty cells (hitsLeft == 0) on the board.
 *
 * Used to check for game over (no space left) or available moves.
 * Iterates over the entire grid.
 *
 * @param board Pointer to the board.
 * @return Anount of empty cell
 */
u32 getEmptyCellCount(const Board_St* const board);

#endif // CORE_GAME_BOARD_H