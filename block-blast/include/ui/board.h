/**
 * @file board.h (ui)
 * @author Fshimi Hawlk
 * @date 2026-01-07
 * @brief Board rendering functions.
 */

#ifndef UI_BOARD_H
#define UI_BOARD_H

#include "utils/userTypes.h"

/**
 * @brief Draws a single block at the specified position with the given color.
 *
 * Includes outline drawing.
 *
 * @param pos The position vector for the block.
 * @param color The color of the block.
 */
void drawBlock(const f32Vector2 pos, const color32 color);

/**
 * @brief Draws the entire game board, rendering each block based on its state.
 *
 * @param board The board structure to draw.
 */
void drawBoard(const Board_St board);

#endif // UI_BOARD_H