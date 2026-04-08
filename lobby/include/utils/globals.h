/**
    @file utils/globals.h
    @author Fshimi-Hawlk
    @date 2026-01-07
    @date 2026-02-23
    @brief Central location for process-wide global variables and pre-loaded resources.

    This header declares variables that are:
        - initialized once during program startup (usually in main() or init function)
        - read (and sometimes modified) from many different parts of the codebase
        - representing fundamental state or resources shared across systems

    Guidelines for usage:
        - Prefer passing context structs (LobbyGame_St, etc.) through function parameters
          whenever reasonable - reduce usage of these globals over time.
        - Only declare truly global / singleton-like items here.
        - Initialization order and thread-safety (currently single-threaded) must be
          carefully managed in the startup sequence.
*/

#ifndef UTILS_GLOBALS_H
#define UTILS_GLOBALS_H

#include "userTypes.h"

// Params menu - include the header for the type definition
#include "include/ui/paramsMenu.h"

// ------------------------------------------------
// Window & display
// ------------------------------------------------

/**
    Rectangle describing the full client area of the application window.
    Usually set to {0, 0, GetScreenWidth(), GetScreenHeight()} after InitWindow().
*/
extern Rectangle windowRect;

// ------------------------------------------------
// Fonts
// ------------------------------------------------

/**
    @brief Default font used for most UI labels, buttons and in-game text.
           Normally corresponds to fonts[FONT16] or similar mid-size variant.
*/
extern Font      appFont;

/**
    @brief Array of pre-loaded fonts at different sizes.
           Indexed by FontSize_Et values (FONT8 … FONT48).
           All fonts should use the same typeface for visual consistency.
*/
extern Font      fonts[_fontSizeCount];

// ------------------------------------------------
// Lobby world content
// ------------------------------------------------

/**
    brief Static array of platform definitions for the lobby scene.
          Size is determined by platformCount.
          @note Consider moving to dynamic allocation or level data file in the future.
*/
extern Platform_st platforms[];

/**
    brief Number of valid entries in the platforms array.
*/
extern u32 platformCount;

// ------------------------------------------------
// Skin selection UI
// ------------------------------------------------

/**
    brief Screen-space rectangle where the "change skin" button is drawn and clickable.
          Used both for rendering and input detection.
*/
extern Rectangle skinButtonRect;

/**
    brief Texture used for the skin selection menu toggle button.
          Usually a gear icon, palette symbol or similar.
*/
extern Texture2D logoSkinButton;

// ------------------------------------------------
// Parameters menu (settings/resolution)
// ------------------------------------------------

/**
    @brief Global parameters menu state (settings button + resolution selector).
*/
extern ParamsMenu_St paramsMenu;

// ------------------------------------------------
// Visual / atmospheric state
// ------------------------------------------------

/** @brief Shared moonlight direction vector (normalized). Used for shadows/glow. */
extern const Vector2 moonLightDir;

/** @brief Pre-loaded tree texture used in the lobby background. */
extern Texture2D texTree;

/** @brief Pre-loaded starry-background texture. */
extern Texture2D texBackground;

/** @brief Platform texture atlas entries (grass, wood plank). */
extern Texture2D platformTextures[2];

/** @brief Accumulated lobby time since launch (seconds). Used for animations. */
extern float gameTime;

/** @brief Grass blade simulation array. */
extern GrassBlade_st grassBlades[MAX_GRASS_BLADES];

/** @brief Number of active grass blades. */
extern int grassCount;

// ------------------------------------------------
// Audio handles
// ------------------------------------------------

extern Sound sound_jump;
extern Sound sound_doubleJump;
extern Sound sound_gameLaunch;
extern Sound meme;

#endif // UTILS_GLOBALS_H
