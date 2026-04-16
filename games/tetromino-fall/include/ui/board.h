/**
    @file board.h
    @author Fshimi-Hawlk
    @date 2026-02-06
    @date 2026-04-14
    @brief UI functions for rendering the Tetris board.
*/

#ifndef UI_BOARD_H
#define UI_BOARD_H

#include "utils/types.h"

/**
    @brief Draws the Tetris board grid and its contents.
    
    Iterates through the board array and renders each cell. 
    Empty cells are drawn with grid lines, while occupied cells are filled with their respective colors.

    @param[in] board The Tetris board state to render.
*/
void tetrominoFall_drawBoard(Board_t board);

#endif