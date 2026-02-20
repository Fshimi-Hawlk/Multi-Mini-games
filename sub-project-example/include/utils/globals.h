/**
 * @file globals.h
 * @author Fshimi-Hawlk
 * @date 2026-01-07
 * @date 2026-02-20
 * @brief Central location for process-wide global variables and pre-loaded resources.
 *
 * This header declares variables that are:
 *   - initialized once during program startup (usually in main() or init function)
 *   - read (and sometimes modified) from many different parts of the codebase
 *   - representing fundamental state or resources shared across systems
 *
 * Guidelines for usage:
 *   - Prefer passing context structs (GameNameGame_St, etc.) through function parameters
 *     whenever reasonable - reduce usage of these globals over time.
 *   - Only declare truly global / singleton-like items here.
 *   - Initialization order and thread-safety (currently single-threaded) must be
 *     carefully managed in the startup sequence.
 *
 * @see main.c / init functions for where most of these globals are set
 */

#ifndef UTILS_GLOBALS_H
#define UTILS_GLOBALS_H

#include "userTypes.h"

// ────────────────────────────────────────────────
// Window & display
// ────────────────────────────────────────────────

/**
 *  Rectangle describing the full client area of the application window.
 *  Usually set to {0, 0, GetScreenWidth(), GetScreenHeight()} after InitWindow().
 */
extern Rectangle windowRect;

// ────────────────────────────────────────────────
// Fonts
// ────────────────────────────────────────────────

/**
 *  @brief Default font used for most UI labels, buttons and in-game text.
 *         Normally corresponds to fonts[FONT16] or similar mid-size variant.
 */
extern Font      appFont;

/**
 *  @brief Array of pre-loaded fonts at different sizes.
 *         Indexed by FontSize_Et values (FONT8 … FONT48).
 *         All fonts should use the same typeface for visual consistency.
 */
extern Font      fonts[_fontSizeCount];

// ────────────────────────────────────────────────
// Game's Globals
// ────────────────────────────────────────────────



#endif // UTILS_GLOBALS_H