/**
 * @file globals.h
 * @author Fshimi Hawlk
 * @author i-Charlys (CAILLON Charles)
 * @date 2026-01-07
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
extern u32 platformCount;               ///< Number of platforms.

extern Rectangle skinButtonRect;        ///< Rectangle for the skin selection button.
extern Texture2D logoSkinButton;        ///< Texture for the skin button logo.

extern bool isTextureMenuOpen;          ///< Flag indicating if the texture selection menu is open.

#endif // UTILS_GLOBALS_H
#include "APIs/chatAPI.h"
extern ChatState_St g_chatState;
