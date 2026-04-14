/**
    @file configs.h
    @author Kimi BERGE
    @date 2026-01-07
    @date 2026-04-14
    @brief Central place for compile-time and tuning constants used throughout the game.
*/
#ifndef CONFIGS_H
#define CONFIGS_H

// 
// Asset paths
// 

#ifndef ASSET_PATH
/**
    @brief Root directory for all game assets (textures, sounds, fonts, etc.).
           Can be overridden at compile time (e.g. -DASSET_PATH="/custom/assets/").
*/
#define ASSET_PATH "../assets/"
#endif

/**
    @brief Path to font assets.
*/
#define FONT_PATH "assets/fonts/"

/**
    @brief Subdirectory containing all image files.
*/
#define IMAGES_PATH ASSET_PATH "images/"

// 
// Window & display
// 

/**
    @brief Default client-area width of the application window (pixels).
*/
#define WINDOW_WIDTH    800

/**
    @brief Default client-area height of the application window (pixels).
*/
#define WINDOW_HEIGHT   700

/**
    @brief Window title shown in the title bar / task manager.
*/
#define WINDOW_TITLE    "Bingo"

/**
    @brief Background clear color used at the start of each frame.
           Dark gray (#121212).
*/
#define APP_BACKGROUND_COLOR ((Color) {196, 215, 165, 255})

// 
// Game Configs
// 

/**
    @brief Maximum number of players in the game.
*/
#define MAX_PLAYER 4


#endif // CONFIGS_H