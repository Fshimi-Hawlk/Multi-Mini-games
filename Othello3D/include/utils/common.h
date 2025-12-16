#ifndef COMMON_H
#define COMMON_H

#pragma region StandardIncludes

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdarg.h>
#include <stdbool.h>

#include <string.h>
#include <ctype.h>
#include <time.h>
#include <math.h>

#pragma endregion StandardIncludes

#pragma region ThirdPartyIncludes

#include "raylib.h"
#include "raymath.h"

#define NOB_STRIP_PREFIX
#include "nob.h"

#pragma endregion ThirdPartyIncludes

#pragma region UserIncludes

#include "logger/logger.h"

#pragma endregion UserIncludes

#endif // COMMON_H