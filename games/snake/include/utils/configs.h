/**
    @file utils/configs.h
    @author Fshimi-Hawlk
    @date 2026-01-07
    @date 2026-02-23
    @brief Central place for compile-time and tuning constants used throughout the game.

    This header defines:
        - Paths to asset directories
        - Window / display settings
        - Visual defaults (colors, font sizes)
        - Core gameplay tuning values (physics, movement, jump mechanics)

    All values here are intended to be easily tweakable without touching logic code.
    When a constant is used in many places or affects feel significantly, it belongs here.

    Guidelines:
        - Prefer named constants over magic numbers in .c files
        - Use this file for values that are unlikely to change per build/environment
        - For runtime-configurable settings (e.g. via file or menu), use globals or a config struct instead
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

#define SIZE_BOARD 20
#define MAX_LENGTH 400

#define CELL_SIZE (WINDOW_HEIGHT / (f32) SIZE_BOARD)

#endif // CONFIGS_H
