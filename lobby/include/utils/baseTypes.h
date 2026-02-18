/**
 * @file baseTypes.h
 * @author Fshimi-Hawlk
 * @date 2026-01-07
 * @date 2026-02-18
 * @brief Fixed-size integer aliases and small vector types used as near-primitive types throughout the project.
 *
 * This header defines:
 *   - unambiguous fixed-width integer types (u8, s32, u64, etc.)
 *   - matching small vector types (mostly 2D) with the same component types
 *
 * Purpose:
 *   - Eliminate platform/size ambiguity when exact bit-width matters (networking, serialization, bitfields, etc.)
 *   - Provide lightweight named vector types that feel like built-ins
 *   - Keep all "primitive-feeling" typedefs in one place (as per project code style)
 *
 * Guidelines:
 *   - Use these types instead of plain int/float when size or signedness is semantically important
 *   - For general-purpose 2D coordinates / velocities prefer Raylib's Vector2 (f32Vector2 is equivalent)
 *   - The various integer-based Vector2 types are mainly useful for grid coordinates, tile indices, texture sizes, etc.
 *
 * @note f32Vector2 is conditionally defined only if not already defined elsewhere (usually in common.h)
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
// 2D vector types (integer components)
// ────────────────────────────────────────────────

typedef struct { u8  x, y; } u8Vector2;   ///< 8-bit unsigned integer coordinates
typedef struct { s8  x, y; } s8Vector2;   ///< 8-bit signed integer coordinates

typedef struct { u16 x, y; } u16Vector2;  ///< 16-bit unsigned (common for texture dimensions, tile indices)
typedef struct { s16 x, y; } s16Vector2;  ///< 16-bit signed

typedef struct { u32 x, y; } u32Vector2;  ///< 32-bit unsigned
typedef struct { s32 x, y; } s32Vector2;  ///< 32-bit signed (common for large grid/world positions)

typedef struct { u64 x, y; } u64Vector2;  ///< 64-bit unsigned (rare — mostly for very large indices)
typedef struct { s64 x, y; } s64Vector2;  ///< 64-bit signed

// ────────────────────────────────────────────────
// 2D vector types (floating-point)
// ────────────────────────────────────────────────

#ifndef f32Vector2_def
/**
 * @brief 32-bit float 2D vector — equivalent to Raylib's Vector2.
 *        Defined here only if not already provided (e.g. via common.h).
 */
typedef struct { f32 x, y; } f32Vector2;
#endif

/**
 * @brief 64-bit float 2D vector (rarely needed — mostly for high-precision calculations).
 */
typedef struct { f64 x, y; } f64Vector2;

#endif // BASE_TYPES_H