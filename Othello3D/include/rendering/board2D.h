#ifndef RENDERING_BOARD2D_H
#define RENDERING_BOARD2D_H

#include "utils/types.h"

/**
 * @brief Draws the board, pieces, and highlights.
 * @param board The game board.
 * @param flipped Array of flipped positions.
 * @param flippedCount Number flipped.
 * @param lastMove Last placement.
 */
void drawGameBoard(const Board_t board,
                   const s64Vector2_St flipped[], u64_t flippedCount,
                   s64Vector2_St lastMove);

#endif // RENDERING_BOARD2D_H


