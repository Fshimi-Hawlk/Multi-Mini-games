/**
    @file utils/common.h
    @author Fshimi-Hawlk
    @date 2026-01-07
    @date 2026-03-20
    @brief Central inclusion point for standard library headers, Raylib, foundational typedefs,
           and most commonly needed project headers.

    Contributors:
        - Fshimi-Hawlk:
            - Added documentation start-up
            - Added general usage libraries

    This header should be included (directly or indirectly via other headers) in nearly
    every .c file in the project. It provides:
        - essential C standard library headers
        - Raylib and related math/rendering headers
        - project-wide base types, configuration constants, and basic utilities
        - dynamic array implementation (nob/dynamicArray.h)

    Design intent:
        - Minimize repeated the includes across source files
        - Establish a predictable set of always-available symbols
        - Keep the set of includes minimal and justified

    Current state notes:
        - Several arena/context and string-related headers are commented out because
          they are not used in the current lobby codebase.
        - When memory management moves beyond simple malloc/free, the REALLOC/FREE
          macros should be re-enabled and pointed to the chosen allocator.
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

#include "rand.h"                // custom PRNG - used in lobby

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

/**
    @brief Shorthand alias for Raylib's Vector2 (used in many places for clarity).
 */
#ifndef f32Vector2_def
typedef Vector2 f32Vector2;
#define f32Vector2_def      // marker to indicate the typedef is active
#endif

#include "baseTypes.h"              // primitive typedefs (u8, f32Vector2, etc.)
#include "configs.h"                // tuning constants, paths, physics values

// ────────────────────────────────────────────────
// Project utilities (currently used / active)
// ────────────────────────────────────────────────

#include "logger.h"                 // logging macros and functions (log_info, log_warn, etc.)
#include "networkInterface.h"


#endif // COMMON_H
