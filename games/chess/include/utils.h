/**
    @file utils.h
    @author Léandre BAUDET
    @date 2024
    @date 2026-04-02
    @brief Utility functions for Echecs.
*/
#ifndef UTILS_H
#define UTILS_H

#include "types.h"

/**
 * @brief Macro to convert board coordinates to algebraic notation.
 * @param x The x coordinate (column)
 * @param y The y coordinate (row)
 * @return Characters representing the position in algebraic notation
 */
#define CBCGC(x, y) (x) + 'a', BOARD_SIZE - (y)

/**
 * @brief Macro to check if coordinates are out of bounds.
 * @param col The column to check
 * @param row The row to check
 * @return true if out of bounds, false otherwise
 */
#define isOOB(col, row) ((col < 0 || col >= BOARD_SIZE || row < 0 || row >= BOARD_SIZE))

/**
 * @brief Print all moves made in the game to stdout.
 */
void printMovesMade(void);

/**
 * @brief Get the current mouse position as integer coordinates.
 * @return IVec2_st containing mouse position
 */
IVec2_st GetMousePositionI(void);

#endif