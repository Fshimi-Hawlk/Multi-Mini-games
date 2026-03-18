/**
 * @file baseTypes.h
 * @author i-Charlys (CAILLON Charles)
 * @date 2026-03-18
 * @brief Basic integer and vector type aliases.
 */

#ifndef BASE_TYPES_H
#define BASE_TYPES_H

#include <stdint.h>

/** @brief Unsigned 8-bit integer. */
typedef uint8_t  u8;
/** @brief Signed 8-bit integer. */
typedef int8_t   s8;
/** @brief Unsigned 16-bit integer. */
typedef uint16_t u16;
/** @brief Signed 16-bit integer. */
typedef int16_t  s16;
/** @brief Unsigned 32-bit integer. */
typedef uint32_t u32;
/** @brief Signed 32-bit integer. */
typedef int32_t  s32;
/** @brief Unsigned 64-bit integer. */
typedef uint64_t u64;
/** @brief Signed 64-bit integer. */
typedef int64_t  s64;

/** @brief 32-bit floating point. */
typedef float    f32;
/** @brief 64-bit floating point. */
typedef double   f64;

/** @brief 2D vector of unsigned 8-bit integers. */
typedef struct {
    u8 x, y;
} u8Vector2;

/** @brief 2D vector of signed 8-bit integers. */
typedef struct {
    s8 x, y;
} s8Vector2;

/** @brief 2D vector of unsigned 16-bit integers. */
typedef struct {
    u16 x, y;
} u16Vector2;

/** @brief 2D vector of signed 16-bit integers. */
typedef struct {
    s16 x, y;
} s16Vector2;

/** @brief 2D vector of unsigned 32-bit integers. */
typedef struct {
    u32 x, y;
} u32Vector2;

/** @brief 2D vector of signed 32-bit integers. */
typedef struct {
    s32 x, y;
} s32Vector2;

/** @brief 2D vector of unsigned 64-bit integers. */
typedef struct {
    u64 x, y;
} u64Vector2;

/** @brief 2D vector of signed 64-bit integers. */
typedef struct {
    s64 x, y;
} s64Vector2;

#ifndef f32Vector2_def
/** @brief 2D vector of 32-bit floating point numbers. */
typedef struct {
    f32 x, y;
} f32Vector2;
#endif

/** @brief 2D vector of 64-bit floating point numbers. */
typedef struct {
    f64 x, y;
} f64Vector2;

#endif // BASE_TYPES_H