/**
 * @file baseTypes.h
 * @author Fshimi-Hawlk
 * @date 2026-01-07
 * @date 2026-02-20
 * @brief Fixed-size integer aliases, small vector types, and common shorthand aliases
 *        used as near-primitive types throughout the entire project (firstparty).
 *
 * This header defines:
 *   - unambiguous fixed-width integer types (u8, s32, u64, etc.)
 *   - matching small vector types (mostly 2D) with the same component types
 *   - convenient shorthand aliases (uint, uchar, iVector2, uVector2, ...) for cases
 *     where exact bit-width is not the primary concern
 *
 * Purpose:
 *   - Eliminate platform/size ambiguity when exact bit-width matters (networking, serialization, bitfields, etc.)
 *   - Provide lightweight named vector types that feel like built-ins
 *   - Offer readable short aliases so team members don't fall back to plain `int`/`float` everywhere
 *   - Keep all "primitive-feeling" typedefs in one shared place
 *
 * @note All types in this file are part of the shared firstparty layer and should be used consistently across
 *       the lobby and every sub-game.
 */

#ifndef BASE_TYPES_H
#define BASE_TYPES_H

#include <stdint.h>
#include <stdbool.h>

// ────────────────────────────────────────────────
// Fixed-width scalar types
// ────────────────────────────────────────────────

typedef uint8_t   u8;
typedef int8_t    s8;

typedef uint16_t  u16;
typedef int16_t   s16;

typedef uint32_t  u32;
typedef int32_t   s32;

typedef uint64_t  u64;
typedef int64_t   s64;

typedef float     f32;
typedef double    f64;

// ────────────────────────────────────────────────
// Convenience shorthand aliases (use when readability > exact size)
// ────────────────────────────────────────────────

typedef unsigned char       uchar;
typedef unsigned short      ushort;
typedef unsigned int        uint;
typedef unsigned long       ulong;
typedef unsigned long long  ullong;

typedef struct
{
    int  x, y;
} iVector2;

typedef struct
{
    uint x, y;
} uVector2;

// ────────────────────────────────────────────────
// 2D vector types (integer components)
// ────────────────────────────────────────────────

typedef struct
{
    u8  x, y;
} u8Vector2;

typedef struct
{
    s8  x, y;
} s8Vector2;

typedef struct
{
    u16 x, y;
} u16Vector2;

typedef struct
{
    s16 x, y;
} s16Vector2;

typedef struct
{
    u32 x, y;
} u32Vector2;

typedef struct
{
    s32 x, y;
} s32Vector2;

typedef struct
{
    u64 x, y;
} u64Vector2;

typedef struct
{
    s64 x, y;
} s64Vector2;

// ────────────────────────────────────────────────
// 2D vector types (floating-point)
// ────────────────────────────────────────────────

#ifndef f32Vector2_def
typedef struct
{
    f32 x, y;
} f32Vector2;
#endif

typedef struct
{
    f64 x, y;
} f64Vector2;

#endif // BASE_TYPES_H
