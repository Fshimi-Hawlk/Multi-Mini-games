/**
 * @file globals.h
 * @author i-Charlys (CAILLON Charles)
 * @date 2026-03-18
 * @brief Global variables and resources.
 */

#ifndef UTILS_GLOBALS_H
#define UTILS_GLOBALS_H

#include "userTypes.h"

extern Rectangle windowRect;            ///< Main window rectangle (set at init).
extern Font      appFont;               ///< Primary font for UI text.
extern Font      fonts[_fontSizeCount]; ///< Array of loaded fonts in increasing sizes.

extern Platform_st platforms[];         ///< Array of platforms in the lobby.
extern int platformCount;               ///< Number of platforms.

extern Texture2D playerTextures[];      ///< Array of available player textures.
extern int playerTextureCount;          ///< Number of player textures.

extern Rectangle defaultPlayerTextureRect; ///< Default source rectangle for player textures.

extern Rectangle skinButtonRect;        ///< Rectangle for the skin selection button.

extern Rectangle kingForFourZone;       ///< Trigger zone for the King For Four game.
extern bool isTextureMenuOpen;          ///< Flag indicating if the texture selection menu is open.

extern Texture2D logoSkinButton;        ///< Texture for the skin button logo.

#endif // UTILS_GLOBALS_H
#include "firstparty/APIs/chatAPI.h"
extern ChatState_St g_chatState;
