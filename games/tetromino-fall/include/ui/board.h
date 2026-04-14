/**
    @file board.h
    @author Léandre BAUDET
    @date 2026-04-14
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
void drawBoard(board_t board);

#endif