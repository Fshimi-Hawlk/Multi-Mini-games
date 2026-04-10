/**
    @file firstparty/utils/mathUtils.h
    @author Fshimi-Hawlk
    @date 2026-04-03
    @date 2026-04-03
    @brief Basic math macros and vector construction helpers.
*/

#ifndef FIRSTPARTY_UTILS_MATHUTILS_H
#define FIRSTPARTY_UTILS_MATHUTILS_H

#include <math.h>

/**
    @brief Returns the sign of a value: -1 if negative, 1 otherwise (0 becomes 1).
*/
#define signof(v) ((v) < 0 ? -1 : 1)

/**
    @brief Standard min/max macros.
*/
#define min(a, b) ((a) < (b) ? (a) : (b))
#define max(a, b) ((a) > (b) ? (a) : (b))

/**
    @brief Converts a number of bytes to KiB/MiB/GiB.
*/
#define KiB(n) ((u64) (n) << 10)
#define MiB(n) ((u64) (n) << 20)
#define GiB(n) ((u64) (n) << 30)

/**
    @brief Aligns a value upward to the nearest multiple of a power-of-2.

    Useful for memory alignment in arenas or buffers.
    Formula: (n + p - 1) & ~(p - 1)

    @note Assumes 'p' is a power of 2; if not, behavior is undefined.
*/
#define alignUpPow2(n, p) (((u64) (n) + (u64) (p) - 1) & (~((u64) (p) - 1)))

/**
    @brief Swaps two variables of the same type using XOR (no temporary needed).
*/
#define swapXor(a, b) do { a ^= b; b ^= a; a ^= b; } while (0)

#define swapT(T, a, b) do { T __temp = a; a = b; b = __temp; } while (0)

/**
    @brief Clamps a value between min and max.
*/
#define clamp(v, _min, _max) min(max((_min), (v)), (_max))

#define lerp(a, b, t) (a) + ((b) - (a)) * (t)

/**
    @brief Constructs a Vector2 by adding/subtracting/multiplying/dividing components.
*/
#define vec2Add(v1, v2, T) (T) { .x = (v1).x + (v2).x, .y = (v1).y + (v2).y }
#define vec2Sub(v1, v2, T) (T) { .x = (v1).x - (v2).x, .y = (v1).y - (v2).y }
#define vec2Mul(v1, v2, T) (T) { .x = (v1).x * (v2).x, .y = (v1).y * (v2).y }
#define vec2Div(v1, v2, T) (T) { .x = (v1).x / (v2).x, .y = (v1).y / (v2).y }

/**
    @brief Constructs a Vector2 by adding/subtracting a value to the components.
*/
#define vec2AddVal(v, val, T) (T) { .x = (v).x + (val), .y = (v).y + (val) }
#define vec2Scale(v, scalar, T) (T) { .x = (v).x * (scalar), .y = (v).y * (scalar) }

/**
    @brief Vector2 helper to print the components.
*/
#define vec2Fmt(vec2) (vec2).x, (vec2).y
#define vec2fStr "%.2f, %.2f"
#define vec2siStr "%d, %d"
#define vec2uiStr "%u, %u"

#endif // FIRSTPARTY_UTILS_MATHUTILS_H