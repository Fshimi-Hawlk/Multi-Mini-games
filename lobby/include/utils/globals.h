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

#include "utils/userTypes.h"
#include "progress.h"

// ────────────────────────────────────────────────
// General Globals
// ────────────────────────────────────────────────

extern LobbyGame_St game;

/**
    @brief Array of pre-loaded fonts at different sizes.
           Indexed by FontSize_Et values (FONT16 … FONT128).
           All fonts should use the same typeface for visual consistency.
*/
extern Font lobby_fonts[__fontSizeCount];

extern f32 gameTime; ///< Accumulated lobby time since launch (seconds). Used for animations.

extern PlayerProgress_St g_progress; ///< Global player progress.
extern Chat_St gameChat;

// ────────────────────────────────────────────────
// Platforms / Terrains / Game Zones
// ────────────────────────────────────────────────

extern TerrainVec_St terrains; ///< List of terrains in the lobby.

/**
    @brief Static array of platform definitions for the lobby scene.
           Size is determined by platformCount.
           @note Consider moving to dynamic allocation or level data file in the future.
*/
extern Platform_St platforms[];

extern u32 platformCount; ///< Number of valid entries in the platforms array.

extern Texture2D platformTextures[__platformTypeCount]; ///< Platform texture atlas entries.

extern GameInteractionZone_St gameZones[__miniGameIdCount];

// ────────────────────────────────────────────────
// Skin selection UI
// ────────────────────────────────────────────────

/**
    @brief Screen-space rectangle where the "change skin" button is drawn and clickable.
           Used both for rendering and input detection.
*/
extern Rectangle skinButtonRect;

/**
    @brief Texture used for the skin selection menu toggle button.
           Usually a gear icon, palette symbol or similar.
*/
extern Texture2D logoSkinButton;

// ────────────────────────────────────────────────
// Parameters menu (settings/resolution)
// ────────────────────────────────────────────────

/**
    @brief Global parameters menu state (settings button + resolution selector).
*/
extern ParamsMenu_St paramsMenu;

// ────────────────────────────────────────────────
// Textures
// ────────────────────────────────────────────────

extern Texture2D treeTexture;
extern Texture2D backgroundTexture;

// ────────────────────────────────────────────────
// Visual / atmospheric state
// ────────────────────────────────────────────────

extern GrassBlade_St grassBlades[MAX_GRASS_BLADES];
extern int grassCount;

extern const Vector2 moonLightDir; ///< Shared moonlight direction vector (normalized). Used for shadows/glow.

// ────────────────────────────────────────────────
// Audio handles
// ────────────────────────────────────────────────

extern Sound sound_jump;
extern Sound sound_doubleJump;
extern Sound sound_gameLaunch;
extern Sound sound_doubleJumpMeme;

// ────────────────────────────────────────────────
// Physics Debug Panel
// ────────────────────────────────────────────────

extern bool showPhysicsDebugPanel; ///< Whether the physics debug panel is visible (toggle with F2).

extern f32 panelScrollY; ///< Scroll position for panels.

#endif // UTILS_GLOBALS_H
