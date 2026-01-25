/**
 * @file configs.h
 * @author Fshimi Hawlk
 * @date 2026-01-07
 * @brief Core configuration constants for the game.
 *
 * These #defines set up window, board, and visual parameters. They're compile-time
 * constants; change here to tweak game feel or resolution.
 */

#ifndef CONFIGS_H
#define CONFIGS_H

#define WINDOW_TITLE "Block Blast"
#define WINDOW_WIDTH 1000
#define WINDOW_HEIGHT 800 // 600

#define APP_BACKGROUND_COLOR (color32) {.r = 18, .g = 18, .b = 18, .a = 255}
#define BOARD_EMPTY_TILE_COLOR (color32) {.r = 22, .g = 16, .b = 79, .a = 255}
#define BLOCK_OUTLINE_COLOR (color32) {.r = 22, .g = 16, .b = 36, .a = 255}

#define APP_TEXT_FONT_SIZE 32       ///< Base text size

#define MAX_SHAPE_SIZE 9    ///< Largest shape size
#define BLOCK_PX_SIZE 50
#define OUTLINE_PX_SIZE ((BLOCK_PX_SIZE) / 10.0f)    ///< Border thickness

#define SCORE_PER_UNIT_PLACED 10
#define SCORE_PER_LINE_CLEAR 100

#define BOARD_WIDTH  8  ///< Grid columns
#define BOARD_HEIGHT 8  ///< Grid rows

#endif // CONFIGS_H