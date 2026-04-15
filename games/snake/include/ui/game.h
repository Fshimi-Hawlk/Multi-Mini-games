/**
    @file game.h
    @author Léandre BAUDET
    @date 2026-04-07
    @date 2026-04-14
    @brief UI and rendering functions for the Snake mini-game.
*/
#ifndef UI_GAME_H
#define UI_GAME_H

#include "utils/userTypes.h"

/**
    @brief Draws the game board and all its tiles (grass, apples, etc.).

    @param[in]  board       The game board to draw.
*/
void snake_drawBoard(const Board_t board);

/**
    @brief Draws the snake with smooth movement interpolation.

    @param[in]  snake           Pointer to the snake structure.
    @param[in]  interpolation   Interpolation factor between frames (0.0 to 1.0).
    @param[in]  direction       Current movement direction of the snake.
*/
void snake_drawSnake(const Snake_St* const snake, f32 interpolation, iVector2 direction);

#endif // UI_GAME_H