/**
    @file rendering.h
    @author Léandre BAUDET
    @date 2024-01-01
    @date 2026-04-14
    @brief Rendering functions for chess.
*/
#ifndef RENDERING_H
#define RENDERING_H

#include "types.h"

/**
    @brief Draw all pieces on the board.
*/
void drawPieces(void);

/**
    @brief Draw a single piece.
    @param[in] piece  The piece to draw
*/
void drawPiece(Piece_st* piece);

/**
    @brief Draw a single cell.
    @param[in] x  The x coordinate of the cell
    @param[in] y  The y coordinate of the cell
    @param[in] c  The color to fill the cell with
*/
void drawCell(int x, int y, Color c);

/**
    @brief Draw the checkerboard pattern.
*/
void drawCheckerboard(void);

/**
    @brief Draw the board border with coordinates.
*/
void drawBorder(void);

/**
    @brief Draw possible move positions for selected piece.
    @param[in] board  The game board
*/
void drawPositionsPossibles(Board_t board);

/**
    @brief Draw the pawn promotion menu.
*/
void printPromotion(void);

/**
    @brief Render a complete frame (clear, draw, display).
    @param[in] board  The game board
*/
void renderFrame(Board_t board);

#endif
