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

// ────────────────────────────────────────────────
// Fixed-width scalar types
// ────────────────────────────────────────────────

typedef uint8_t   u8;     ///< 8-bit unsigned integer
typedef int8_t    s8;     ///< 8-bit signed integer

typedef uint16_t  u16;    ///< 16-bit unsigned integer
typedef int16_t   s16;    ///< 16-bit signed integer

typedef uint32_t  u32;    ///< 32-bit unsigned integer
typedef int32_t   s32;    ///< 32-bit signed integer

typedef uint64_t  u64;    ///< 64-bit unsigned integer
typedef int64_t   s64;    ///< 64-bit signed integer

typedef float     f32;    ///< 32-bit IEEE 754 floating-point
typedef double    f64;    ///< 64-bit IEEE 754 floating-point

// ────────────────────────────────────────────────
// Convenience shorthand aliases (use when readability > exact size)
// ────────────────────────────────────────────────

typedef unsigned char       uchar;   ///< Shorthand for unsigned char
typedef unsigned short      ushort;  ///< Shorthand for unsigned short
typedef unsigned int        uint;    ///< Shorthand for unsigned int
typedef unsigned long       ulong;   ///< Shorthand for unsigned long
typedef unsigned long long  ullong;  ///< Shorthand for unsigned long long

typedef struct { int  x, y; } iVector2;   ///< Signed int 2D vector - convenient when exact 32-bit width is not required.
typedef struct { uint x, y; } uVector2;   ///< Unsigned int 2D vector - convenient when exact 32-bit width is not required.

// ────────────────────────────────────────────────
// 2D vector types (integer components)
// ────────────────────────────────────────────────

typedef struct { u8  x, y; } u8Vector2;   ///< 8-bit unsigned integer coordinates
typedef struct { s8  x, y; } s8Vector2;   ///< 8-bit signed integer coordinates

typedef struct { u16 x, y; } u16Vector2;  ///< 16-bit unsigned (common for texture dimensions, tile indices)
typedef struct { s16 x, y; } s16Vector2;  ///< 16-bit signed

typedef struct { u32 x, y; } u32Vector2;  ///< 32-bit unsigned
typedef struct { s32 x, y; } s32Vector2;  ///< 32-bit signed (common for large grid/world positions)

typedef struct { u64 x, y; } u64Vector2;  ///< 64-bit unsigned (rare - mostly for very large indices)
typedef struct { s64 x, y; } s64Vector2;  ///< 64-bit signed

// ────────────────────────────────────────────────
// 2D vector types (floating-point)
// ────────────────────────────────────────────────

#ifndef f32Vector2_def
/**
 * @brief 32-bit float 2D vector - equivalent to Raylib's Vector2.
 *        Defined here only if not already provided (e.g. via common.h).
 */
typedef struct { f32 x, y; } f32Vector2;
#endif

/**
 * @brief 64-bit float 2D vector (rarely needed - mostly for high-precision calculations).
 */
typedef struct { f64 x, y; } f64Vector2;

#endif // BASE_TYPES_H