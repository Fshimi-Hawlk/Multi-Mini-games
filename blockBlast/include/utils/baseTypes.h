#ifndef BASE_TYPES_H
#define BASE_TYPES_H

#include <stdint.h>

typedef uint8_t  u8;
typedef int8_t   s8;
typedef uint16_t u16;
typedef int16_t  s16;
typedef uint32_t u32;
typedef int32_t  s32;
typedef uint64_t u64;
typedef int64_t  s64;

typedef float    f32;
typedef double   f64;

typedef struct {
    u8 x, y;
} u8Vector2;

typedef struct {
    s8 x, y;
} s8Vector2;

typedef struct {
    u16 x, y;
} u16Vector2;

typedef struct {
    s16 x, y;
} s16Vector2;

typedef struct {
    u32 x, y;
} u32Vector2;

typedef struct {
    s32 x, y;
} s32Vector2;

typedef struct {
    u64 x, y;
} u64Vector2;

typedef struct {
    s64 x, y;
} s64Vector2;

typedef struct {
    f32 x, y;
} f32Vector2;

typedef struct {
    f64 x, y;
} f64Vector2;

#endif // BASE_TYPES_H