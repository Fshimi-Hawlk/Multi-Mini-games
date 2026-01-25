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
* @brief Renders the game board, including all blocks and empty tiles.
 *
 * Iterates over the grid, choosing colors based on block state:
 * - Empty (hitsLeft == 0): BOARD_EMPTY_TILE_COLOR
 * - Removed (hitsLeft < 0): APP_BACKGROUND_COLOR (faded)
 * - Active: From blockColors[], potentially brightness-adjusted by hitsLeft.
 *
 * @param board The board structure to draw.
 */
void drawBoard(const Board_St board);

#endif // UI_BOARD_H