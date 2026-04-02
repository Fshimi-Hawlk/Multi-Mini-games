/**
    @file utils/globals.h
    @author Fshimi-Hawlk
    @author i-Charlys
    @date 2026-01-07
    @date 2026-03-18
    @brief Central location for process-wide global variables and pre-loaded resources.
*/

#ifndef UTILS_GLOBALS_H
#define UTILS_GLOBALS_H

#include "userTypes.h"
#include "configs.h"

/**
    @brief Array of pre-loaded fonts at different sizes.
*/
extern Font lobby_fonts[__fontSizeCount];

/** @brief Main lobby game state. */
extern LobbyGame_St lobby_game;

/** @brief Global player progress. */
extern PlayerProgress_St g_progress;

/** @brief List of terrains in the lobby. */
extern TerrainVec_St terrains;

/** @brief Interaction zones for mini-games. */
extern GameInteractionZone_St gameInteractionZones[__miniGameCount];

/** @brief Global chat state. */
extern Chat_St gameChat;

/** @brief Skin selection button texture. */
extern Texture2D logoSkinButton;

/** @brief Skin button screen rectangle. */
extern Rectangle skinButtonRect;

#endif // UTILS_GLOBALS_H
