/**
    @file globals.h
    @author Fshimi-Hawlk
    @date 2026-01-25
    @date 2026-03-16
    @brief Central location for process-wide global variables and pre-loaded resources.
*/

#ifndef UTILS_GLOBALS_H
#define UTILS_GLOBALS_H

#include "userTypes.h"

// ────────────────────────────────────────────────
// Fonts
// ────────────────────────────────────────────────

/**
     @brief Array of pre-loaded fonts at different sizes.
            Indexed by FontSize_Et values (FONT8 … FONT48).
            All fonts should use the same typeface for visual consistency.
*/
extern Font      fonts[__fontSizeCount];

// ────────────────────────────────────────────────
// Game's Globals
// ────────────────────────────────────────────────

extern const char* const LETTERS[5];

#endif // UTILS_GLOBALS_H