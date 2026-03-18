/**
 * @file common.h
 * @author i-Charlys (CAILLON Charles)
 * @date 2026-03-18
 * @brief Common includes, typedefs, and configuration headers.
 */

#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdbool.h>

#include <string.h>
#include <ctype.h>
#include <time.h>
#include <math.h>
#include <assert.h>

#include "logger.h"

#include "nob/dynamicArray.h"
#include "nob/stringBuilder.h"

#include "raylib.h"
#include "raymath.h"
#include "rlgl.h"

/**
 * @brief Typedef for Vector2 using f32.
 */
typedef Vector2 f32Vector2;
#define f32Vector2_def

/**
 * @brief Typedef for Color as ColorRGBA_St.
 */
typedef Color ColorRGBA_St;

/**
 * @brief Alias for ColorRGBA_St.
 */
typedef ColorRGBA_St color32;

#include "baseTypes.h"
#include "configs.h"

#endif // COMMON_H