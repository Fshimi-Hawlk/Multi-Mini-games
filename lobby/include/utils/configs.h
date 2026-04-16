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

#define FONTS_PATH "assets/fonts/"          ///< `assets` shared folder is not to be confused lobby's own assets

#define IMAGES_PATH ASSET_PATH "images/"        ///< Subdirectory containing all image files.
#define SKINS_PATH  IMAGES_PATH "skins/"        ///< Subdirectory containing all texture files.
#define TEXTURES_PATH IMAGES_PATH "textures/"   ///< Subdirectory containing all texture files.
#define SOUNDS_PATH ASSET_PATH "sounds/"        ///< Subdirectory containing all sound files.

// ────────────────────────────────────────────────
// Window & display
// ────────────────────────────────────────────────

#define WINDOW_WIDTH    1280         ///< Default client-area width of the application window (pixels).
#define WINDOW_HEIGHT   720         ///< Default client-area height of the application window (pixels).

#define WINDOW_TITLE    "Lobby"     ///< Window title shown in the title bar / task manager.

/**
    @brief Background clear color used at the start of each frame.
           Dark gray (#121212).
*/
#define APP_BACKGROUND_COLOR ((Color){18, 18, 18, 255})

#endif // CONFIGS_H
