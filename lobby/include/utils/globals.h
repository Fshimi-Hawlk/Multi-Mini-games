/**
    @file utils/globals.h
    @author Fshimi-Hawlk
    @author i-Charlys (CAILLON Charles)
    @date 2026-01-07
    @date 2026-03-18
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

/**
    @brief Array of pre-loaded fonts at different sizes.
           Indexed by FontSize_Et values (FONT8 … FONT48).
           All fonts should use the same typeface for visual consistency.
*/
extern Font lobby_fonts[__fontSizeCount];

extern LobbyGame_St lobby_game;

// ────────────────────────────────────────────────
// Lobby world content
// ────────────────────────────────────────────────

/**
    @brief Static array of platform definitions for the lobby scene.
          Size is determined by platformCount.
          @note Consider moving to dynamic allocation or level data file in the future.
*/
extern TerrainVec_St terrains;

extern GameInteractionZone_St gameInteractionZones[__miniGameCount];

// ────────────────────────────────────────────────
// Skin selection UI
// ────────────────────────────────────────────────

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

#endif // UTILS_GLOBALS_H
