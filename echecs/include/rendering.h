/**
 * @file rendering.h
 * @author Maxime CHAUVEAU
 * @brief Rendering functions for Echecs.
 * @version 1.0
 * @date 2024
 *
 * This file contains all the rendering functions for drawing
 * the game board, pieces, and UI elements.
 */

#ifndef RENDERING_H
#define RENDERING_H

#include "types.h"

/**
 * @brief Draw all pieces on the board.
 */
void drawPieces(void);

/**
 * @brief Draw a single piece.
 * @param piece The piece to draw
 */
void drawPiece(Piece_st* piece);

/**
 * @brief Draw a single cell.
 * @param x The x coordinate of the cell
 * @param y The y coordinate of the cell
 * @param c The color to fill the cell with
 */
void drawCell(int x, int y, Color c);

/**
 * @brief Draw the checkerboard pattern.
 */
void drawCheckerboard(void);

/**
 * @brief Draw the board border with coordinates.
 */
void drawBorder(void);

/**
 * @brief Draw possible move positions for selected piece.
 * @param board The game board
 */
void drawPositionsPossibles(Board_t board);

/**
 * @brief Draw the pawn promotion menu.
 */
void printPromotion(void);

/**
 * @brief Render a complete frame (clear, draw, display).
 * @param board The game board
 */
void renderFrame(Board_t board);

#endif