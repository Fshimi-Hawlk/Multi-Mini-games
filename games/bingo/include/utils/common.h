/**
    @file common.h
    @author Kimi BERGE
    @date 2026-01-07
    @date 2026-04-14
    @brief Central inclusion point for standard library headers, Raylib, foundational typedefs,
*/
#ifndef COMMON_H
#define COMMON_H

// ────────────────────────────────────────────────
// C Standard Library
// ────────────────────────────────────────────────

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdbool.h>

#include <string.h>
#include <time.h>
#include <math.h>
#include <assert.h>
#include <ctype.h>
#include <fcntl.h>

// ────────────────────────────────────────────────
// Third-party / single-header libraries
// ────────────────────────────────────────────────

#include "contextArena.h"

#define REALLOC context_realloc     // future arena-aware realloc (disabled for now)
#define FREE                        // future arena-aware free       (disabled for now)

#include "nob/dynamicArray.h"       // type-safe dynamic array macros (da_* family)

// #include "nob/stringBuilder.h"   // string builder - not used in current codebase
// #include "stringView.h"          // lightweight string view - not used in current codebase

// ────────────────────────────────────────────────
// Raylib core + helpers
// ────────────────────────────────────────────────

#include "raylib.h"                 // main Raylib API (drawing, input, window, audio, …)
#include "raymath.h"                // vector/matrix/math helpers
#include "rlgl.h"                   // low-level OpenGL abstraction (used rarely)

// ────────────────────────────────────────────────
// Project foundational headers
// ────────────────────────────────────────────────

#ifndef f32Vector2_def
#define f32Vector2_def      // marker to indicate the typedef is active
/**
    @brief Shorthand alias for Raylib's Vector2 (used in many places for clarity).
 */
typedef Vector2 f32Vector2;
#endif // f32Vector2_def

#include "configs.h"                // tuning constants, paths, physics values
#include "baseTypes.h"              // primitive typedefs (u8, f32Vector2, etc.)

// ────────────────────────────────────────────────
// Project utilities
// ────────────────────────────────────────────────

#include "logger.h"                 // logging macros and functions (log_info, log_warn, etc.)
#include "networkInterface.h"
#include "systemSettings.h"
#include "paramsMenu.h"
#include "APIs/generalAPI.h"
#include "APIs/chatAPI.h"

#endif // COMMON_H
