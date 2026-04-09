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

// ------------------------------------------------
// Asset paths
// ------------------------------------------------

#ifndef ASSET_PATH
/**
    @brief Root directory for all game assets (textures, sounds, fonts, etc.).
           Can be overridden at compile time (e.g. -DASSET_PATH="/custom/assets/").
*/
#define ASSET_PATH "assets/"
#endif

#define IMAGES_PATH ASSET_PATH "images/"    ///< Subdirectory containing all image files.
#define SKINS_PATH  IMAGES_PATH "skins/"    ///< Subdirectory containing all texture files.
#define SOUNDS_PATH ASSET_PATH "sounds/"    ///< Subdirectory containing all sound files.

// ------------------------------------------------
// Window & display
// ------------------------------------------------

#define WINDOW_WIDTH    800         ///< Default client-area width of the application window (pixels).
#define WINDOW_HEIGHT   600         ///< Default client-area height of the application window (pixels).

#define WINDOW_TITLE    "Lobby"     ///< Window title shown in the title bar / task manager.

/**
    @brief Background clear color used at the start of each frame.
           Dark gray (#121212).
*/
#define APP_BACKGROUND_COLOR ((Color){18, 18, 18, 255})

/**
    @brief Default base font size for most UI text and in-game labels.
           Other sizes are usually derived from this via the fonts[] array.
*/
#define APP_TEXT_FONT_SIZE 32

#endif // CONFIGS_H
