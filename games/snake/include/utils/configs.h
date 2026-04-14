/**
    @file configs.h
    @author Léandre BAUDET
    @date 2026-01-07
    @date 2026-04-14
    @brief Central place for compile-time and tuning constants used throughout the game.
*/
#ifndef CONFIGS_H
#define CONFIGS_H

// ────────────────────────────────────────────────
// Asset paths
// ────────────────────────────────────────────────

#ifndef ASSET_PATH
/**
    @brief Root directory for all game assets (textures, sounds, fonts, etc.).
           Can be overridden at compile time (e.g. -DASSET_PATH="/custom/assets/").
*/
#define ASSET_PATH "assets/"
#endif

#define IMAGES_PATH ASSET_PATH "images/"            ///< Subdirectory containing all image files.

// ────────────────────────────────────────────────
// Window & display
// ────────────────────────────────────────────────

#define WINDOW_WIDTH    800        ///< Default client-area width of the application window (pixels).
#define WINDOW_HEIGHT   600         ///< Default client-area height of the application window (pixels).

#define WINDOW_TITLE    "Snake"     ///< Window title shown in the title bar / task manager.

/**
    @brief Background clear color used at the start of each frame.
*/
#define APP_BACKGROUND_COLOR WHITE

// ────────────────────────────────────────────────
// Physics & movement tuning (lobby platformer)
// ────────────────────────────────────────────────

#define SIZE_BOARD 20               ///< The size of the square game board (number of cells per side).
#define MAX_LENGTH 400              ///< The maximum possible length of the snake.

#define CELL_SIZE (WINDOW_HEIGHT / (f32) SIZE_BOARD) ///< The size of each cell in pixels.

#endif // CONFIGS_H
