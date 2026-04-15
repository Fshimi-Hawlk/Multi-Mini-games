/**
    @file configs.h
    @author Kimi BERGE
    @date 2026-01-07
    @date 2026-04-14
    @brief Core configuration constants for the game.
*/
#ifndef CONFIGS_H
#define CONFIGS_H

/**
    @brief Path to the fonts directory.
*/
#define FONT_PATH "assets/fonts/"

#ifndef ASSET_PATH
/**
    @brief Root directory for all game assets.
*/
#define ASSET_PATH "assets/"
#endif

/**
    @brief Path to the sounds directory.
*/
#define SOUNDS_PATH ASSET_PATH "sounds/"

/**
    @brief Path to the saves directory.
*/
#define SAVES_PATH ASSET_PATH "saves/"

#define WINDOW_TITLE "Block Blast"      ///< Window title string.
#define WINDOW_WIDTH 1000               ///< Initial window width in pixels.
#define WINDOW_HEIGHT 800               ///< Initial window height in pixels.

/**
    @brief Background color for the application.
*/
#define APP_BACKGROUND_COLOR (Color) {.r = 18, .g = 18, .b = 18, .a = 255}

/**
    @brief Color for empty tiles on the board.
*/
#define BOARD_EMPTY_TILE_COLOR (Color) {.r = 22, .g = 16, .b = 79, .a = 255}

/**
    @brief Color for block outlines.
*/
#define BLOCK_OUTLINE_COLOR (Color) {.r = 22, .g = 16, .b = 36, .a = 255}

#define APP_TEXT_FONT_SIZE 32       ///< Base text size in pixels.

#define MAX_SHAPE_SIZE 9            ///< Largest shape size (max number of blocks).
#define BLOCK_PX_SIZE 50            ///< Visual size of a single block in pixels.
#define OUTLINE_PX_SIZE ((BLOCK_PX_SIZE) / 10.0f)    ///< Border thickness in pixels.

#define SCORE_PER_UNIT_PLACED 10    ///< Points awarded for placing a single block.
#define SCORE_PER_LINE_CLEAR 100    ///< Points awarded for clearing a full line.

#define BOARD_WIDTH  8              ///< Number of columns in the grid.
#define BOARD_HEIGHT 8              ///< Number of rows in the grid.

#endif // CONFIGS_H