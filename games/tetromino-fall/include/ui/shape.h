/**
    @file shape.h
    @author Fshimi-Hawlk
    @date 2026-02-06
    @date 2026-04-14
    @brief UI functions for rendering Tetris shapes.
*/
#ifndef UI_SHAPE_H
#define UI_SHAPE_H

#include "utils/types.h"

/**
    @brief Draws the current falling shape on the board.
    
    Renders each block of the given tetromino at its current board position.

    @param[in] boardShape The current shape state (type, position, rotation).
*/
void tetris_drawShape(boardShape_st boardShape);

/**
    @brief Draws the preview of the next shape to appear.
    
    Renders the next tetromino in a dedicated preview area.

    @param[in] boardShape The next shape state to preview.
*/
void drawNextShape(boardShape_st boardShape);

#endif