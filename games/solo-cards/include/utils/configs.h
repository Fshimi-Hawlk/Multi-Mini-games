/**
    @file configs.h
    @author Maxime CHAUVEAU
    @date 2026-02-01
    @date 2026-04-14
    @brief Configuration constants for Solitaire
*/
#ifndef UTILS_CONFIGS_H
#define UTILS_CONFIGS_H

#ifndef ASSET_PATH
/**
    @brief Root directory for all game assets.
*/
#define ASSET_PATH "assets/"
#endif

#define WINDOW_WIDTH    800             ///< Window width
#define WINDOW_HEIGHT   600             ///< Window height
#define WINDOW_NAME     "Solitaire"     ///< Window name

#define BOARD_WIDTH 10                  ///< Board width
#define BOARD_HEIGHT 20                 ///< Board height

#define CELL_SIZE 25                    ///< Cell size
#define RGB(r, g, b) (Color) {r, g, b, 255}             ///< Helper macro to create Color
#define BACKGROUND_COLOR RGB(0, 0, 30)                  ///< Background color
#define BOARD_BACKGROUND_COLOR RGB(50, 50, 50)          ///< Board background color
#define BOARD_GRID_COLOR RGB(100, 100, 100)             ///< Board grid color

#define CYAN RGB(0, 255, 255)           ///< Cyan color

#define SOUNDS_PATH "assets/sounds/"    ///< Path to sounds assets

#endif // UTILS_CONFIGS_H