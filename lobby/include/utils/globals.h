/**
 * @file globals.h
 * @author Fshimi Hawlk
 * @date 2026-01-07
 * @brief Global variables and resources.
 */

#ifndef UTILS_GLOBALS_H
#define UTILS_GLOBALS_H

#include "userTypes.h"

extern Rectangle windowRect;            ///< Main window rectangle (set at init).
extern Font      appFont;               ///< Primary font for UI text.
extern Font      fonts[_fontSizeCount]; ///< Array of loaded fonts in increasing sizes.


extern Platform_st platforms[];
extern int platformCount;

extern Texture2D playerTextures[];
extern int playerTextureCount;

extern Rectangle defaultPlayerTextureRect;

extern Rectangle skinButtonRect;

extern bool isTextureMenuOpen;

extern Texture2D logoSkinButton;

#endif // UTILS_GLOBALS_H