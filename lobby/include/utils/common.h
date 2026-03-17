/**
    @file utils/common.h
    @author Maxime CHAUVEAU
    @date 2026-02-25
    @brief Central inclusion point for standard library headers, Raylib, foundational typedefs,
           and most commonly needed project headers.

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
#include <ctype.h>
#include <time.h>
#include <math.h>
#include <assert.h>

// ────────────────────────────────────────────────
// Project utilities (currently used / active)
// ────────────────────────────────────────────────

#include "logger.h"                 // logging macros and functions (log_info, log_warn, etc.)

// ────────────────────────────────────────────────
// Third-party / single-header libraries
// ────────────────────────────────────────────────

#include "nob/dynamicArray.h"       // type-safe dynamic array macros (da_* family)

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
typedef Vector2 f32Vector2;
#define f32Vector2_def      // marker to indicate the typedef is active

#include "baseTypes.h"              // primitive typedefs (u8, f32Vector2, etc.)
#include "configs.h"                // tuning constants, paths, physics values

#endif // COMMON_H
