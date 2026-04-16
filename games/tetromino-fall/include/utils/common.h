/**
    @file common.h
    @author Fshimi-Hawlk
    @date 2026-02-06
    @date 2026-04-14
    @brief Common includes and basic type aliases for the Tetromino Fall game.
*/
#ifndef UTILS_COMMON_H
#define UTILS_COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include <string.h>
#include <time.h>
#include <math.h>

#include "raylib.h"

/**
    @brief Shorthand alias for Raylib's Vector2 (used in many places for clarity).
 */
typedef Vector2 f32Vector2;
#define f32Vector2_def      // marker to indicate the typedef is active

#include "baseTypes.h"              // primitive typedefs (u8, f32Vector2, etc.)
#include "configs.h"

#endif
