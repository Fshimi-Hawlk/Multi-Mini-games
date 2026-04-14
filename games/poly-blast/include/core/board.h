/**
    @file board.h
    @author Kimi BERGE
    @date 2026-01-07
    @date 2026-04-14
    @brief Board logic: bounds checking and line clearing.
*/
#ifndef CORE_GAME_BOARD_H
#define CORE_GAME_BOARD_H

#include "utils/userTypes.h"

/**
    @brief Checks if a position is within the board bounds.

    @param[in]     pos          The position to check.
    @param[in]     board        Pointer to the board.
    @return                     true if in bounds, false otherwise.
*/
bool polyBlast_isInBound(const s8Vector2 pos, const Board_St* const board);

/**
    @brief Finds all valid anchor positions for a given shape on the board.

    @param[in]     board        Pointer to the board.
    @param[in]     shape        Pointer to the shape to place.
    @return                     Vector of valid anchor positions.
*/
AnchorVec_St polyBlast_getAnchorCandidates(const Board_St* const board, const Shape_St* const shape);

/**
    @brief Scans the board and marks full rows and columns for clearing.

    A row/column is considered full when **every** cell has hitsLeft > 0.
    Empty cells (hitsLeft == 0) disqualify the line.

    Results are written into board->rowsToClear[] and board->columnsToClear[].
    These arrays **must** be valid and zero-initialized before calling this function.

    @param[in,out] board        Pointer to the board.
*/
void polyBlast_updateBoardClearing(Board_St* const board);

/**
    @brief Scans the board's rows/cols to clear arrays.

    @param[in]     board        Pointer to the board.
    @return                     true if at least one row **or** column is full.
    @return                     false if the board has no completable lines.
*/
bool polyBlast_checkBoardForClearing(const Board_St* const board);

/**
    @brief Clears marked rows and columns on the board.

    @param[in,out] board        Pointer to the board.
*/
void polyBlast_clearBoard(Board_St* const board);

/**
    @brief Counts empty cells (hitsLeft == 0) on the board.

    Used to check for game over (no space left) or available moves.
    Iterates over the entire grid.

    @param[in]     board        Pointer to the board.
    @return                     Amount of empty cells.
*/
u32 polyBlast_getEmptyCellCount(const Board_St* const board);

/**
    @brief Prints the positions of empty cells to the debug log.

    @param[in]     cells        Vector of anchor positions.
*/
void polyBlast_printEmptyCells(const AnchorVec_St cells);

/**
    @brief Prints the current state of the board to the console.

    @param[in]     board        Pointer to the board.
*/
void polyBlast_printBoard(const Board_St* const board);

#endif // CORE_GAME_BOARD_H