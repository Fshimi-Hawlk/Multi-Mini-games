#ifndef RENDERING_BOARD3D_H
#define RENDERING_BOARD3D_H

#include "utils/types.h"

/**
 * @brief Draws the 3D board, grid, pieces, and move highlights.
 *
 * Renders the ground plane, grid lines, all placed pieces as cylinders,
 * and highlights the last move (red torus) and flipped pieces (orange tori).
 *
 * @param board         The current game board state.
 * @param lastMove      Position of the most recent move (or {-1,-1} if none).
 * @param flippedPieces Array of positions of pieces flipped on the last turn.
 * @param flippedCount  Number of entries in flippedPieces array.
 */
void draw3DBoard(const Board_t board, s64Vector2_St lastMove, const s64Vector2_St flippedPieces[], u64_t flippedCount);

#endif // RENDERING_BOARD3D_H