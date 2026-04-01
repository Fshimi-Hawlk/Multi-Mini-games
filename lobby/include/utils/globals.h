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
#include "configs.h"

extern Rectangle windowRect;            ///< Main window rectangle (set at init).
extern Font      appFont;               ///< Primary font for UI text.
extern Font      fonts[__fontSizeCount]; ///< Array of loaded fonts in increasing sizes.


extern Platform_St platforms[];
extern u32 platformCount;

#include "APIs/chatAPI.h"
extern Chat_St gameChat;

extern Texture2D playerTextures[];
extern int playerTextureCount;

extern Rectangle defaultPlayerTextureRect;

extern Rectangle skinButtonRect;

extern bool isTextureMenuOpen;

extern Texture2D logoSkinButton;

extern Rectangle kingForFourZone;

#endif // UTILS_GLOBALS_H
