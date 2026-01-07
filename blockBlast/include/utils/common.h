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

#include "rand.h"
#include "logger.h"
#include "contextArena.h"

#define REALLOC context_realloc
#define FREE

#include "nob/dynamicArray.h"
#include "nob/stringBuilder.h"
#include "stringView.h"

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