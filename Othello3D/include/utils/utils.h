#ifndef UTILS_H
#define UTILS_H

#include "types.h"

#pragma region Defines

#define INVALID_PTR (void *) -1

#pragma endregion Defines

#pragma region Macros

#define dcall log_debug("called")

#define signof(v) ((v) < 0 ? -1 : 1)
#define clamp(v, min, max) v = (v) < (min) ? (min) : (v) > (max) ? (max) : (v)

#pragma endregion Macros

#pragma region Prototypes

#pragma endregion Prototypes

#endif // UTILS_H