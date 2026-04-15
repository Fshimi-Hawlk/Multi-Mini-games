/**
    @file globals.h
    @author Léandre BAUDET
    @date 2026-01-07
    @date 2026-04-14
    @brief Central location for process-wide global variables and pre-loaded resources.
*/
#ifndef UTILS_GLOBALS_H
#define UTILS_GLOBALS_H

#include "utils/userTypes.h"
#include "progress.h"

// ────────────────────────────────────────────────
// General Globals
// ────────────────────────────────────────────────

extern LobbyGame_St lobby_game;

/**
    @brief Array of pre-loaded fonts at different sizes.
           Indexed by FontSize_Et values (FONT16 … FONT128).
           All fonts should use the same typeface for visual consistency.
*/
extern Font lobby_fonts[__fontSizeCount];

extern f32 gameTime; ///< Accumulated lobby time since launch (seconds). Used for animations.

extern PlayerProgress_St g_progress; ///< Global player progress.

// ────────────────────────────────────────────────
// Platforms / Terrains / Game Zones
// ────────────────────────────────────────────────

extern TerrainVec_St terrains; ///< List of terrains in the lobby.

extern Texture2D terrainTextures[__terrainKindCount]; ///< Platform texture atlas entries.

/**
    @brief Global variable gameZones[__miniGameIdCount]
*/
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
extern Texture2D leafTexture;

// ────────────────────────────────────────────────
// Visual / atmospheric state
// ────────────────────────────────────────────────

extern GrassBlade_St grassBlades[MAX_GRASS_BLADES];
/**
    @brief Global variable grassCount
*/
extern int grassCount;

extern const Vector2 moonLightDir; ///< Shared moonlight direction vector (normalized). Used for shadows/glow.

// ────────────────────────────────────────────────
// Audio handles
// ────────────────────────────────────────────────

extern Sound sound_jump;
/**
    @brief Global variable sound_doubleJump
*/
extern Sound sound_doubleJump;
/**
    @brief Global variable sound_gameLaunch
*/
extern Sound sound_gameLaunch;
/**
    @brief Global variable sound_doubleJumpMeme
*/
extern Sound sound_doubleJumpMeme;

// ────────────────────────────────────────────────
// Physics Debug Panel
// ────────────────────────────────────────────────

extern bool showPhysicsDebugPanel; ///< Whether the physics debug panel is visible (toggle with F2).

extern f32 panelScrollY; ///< Scroll position for panels.

#endif // UTILS_GLOBALS_H
