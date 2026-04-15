/**
    @file configs.h
    @author Fshimi-Hawlk
    @date 2026-02-06
    @date 2026-04-14
    @brief Configuration constants for the Tetromino Fall game.
*/
#ifndef UTILS_CONFIGS_H
#define UTILS_CONFIGS_H

#ifndef ASSET_PATH
/**
    @brief Root directory for all game assets.
*/
#define ASSET_PATH "assets/"
#endif

#define WINDOW_WIDTH    400             ///< Game window width in pixels.
#define WINDOW_HEIGHT   800             ///< Game window height in pixels.
#define WINDOW_NAME     "Tetris"        ///< Game window title.

#define BOARD_WIDTH 10                  ///< Tetris board width in cells.
#define BOARD_HEIGHT 20                 ///< Tetris board height in cells.

#define CELL_SIZE 25                    ///< Size of each cell in pixels.

/**
    @brief Macro to create a Color from RGB values with 255 alpha.
*/
#define RGB(r, g, b) (Color) {r, g, b, 255}

#define BACKGROUND_COLOR RGB(0, 0, 0)           ///< Main background color.
#define BOARD_BACKGROUND_COLOR RGB(50, 50, 50)  ///< Board background color.
#define BOARD_GRID_COLOR RGB(100, 100, 100)     ///< Board grid line color.

#define CYAN RGB(0, 255, 255)                   ///< Cyan color for pieces.

#endif
