/**
 * @file common.h
 * @author Fshimi Hawlk
 * @date 2026-01-07
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

#ifndef _USE_DEFAULT_RAND
#include "rand.h"

#define RAND_FUNC prng_rand
#else
#define RAND_FUNC rand
#endif

#include "logger.h"

#ifndef _USE_DEFAULT_ALLOC
#include "contextArena.h"

#define REALLOC context_realloc
#define FREE
#endif

#include "nob/dynamicArray.h"
#include "nob/stringBuilder.h"
// #include "stringView.h"

#include "raylib.h"
#include "raymath.h"
#include "rlgl.h"

typedef Vector2 f32Vector2;
#define f32Vector2_def

typedef Color ColorRGBA_St;
typedef ColorRGBA_St color32;

#include "baseTypes.h"
#include "configs.h"

#endif // COMMON_H