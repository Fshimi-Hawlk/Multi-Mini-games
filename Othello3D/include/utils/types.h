#ifndef TYPES_H
#define TYPES_H

#include "common.h"
#include "config.h"

typedef uint8_t  u8_t;
typedef int8_t   s8_t;
typedef uint16_t u16_t;
typedef int16_t  s16_t;
typedef uint32_t u32_t;
typedef int32_t  s32_t;
typedef uint64_t u64_t;
typedef int64_t  s64_t;

typedef float    f32_t;
typedef double   f64_t;

typedef struct u8Vector2_s {
    u8_t x, y;
} u8Vector2_St, *u8Vector2Ptr_St;

typedef struct s8Vector2_s {
    s8_t x, y;
} s8Vector2_St, *s8Vector2Ptr_St;

typedef struct u16Vector2_s {
    u16_t x, y;
} u16Vector2_St, *u16Vector2Ptr_St;

typedef struct s16Vector2_s {
    s16_t x, y;
} s16Vector2_St, *s16Vector2Ptr_St;

typedef struct u32Vector2_s {
    u32_t x, y;
} u32Vector2_St, *u32Vector2Ptr_St;

typedef struct s32Vector2_s {
    s32_t x, y;
} s32Vector2_St, *s32Vector2Ptr_St;

typedef struct u64Vector2_s {
    u64_t x, y;
} u64Vector2_St, *u64Vector2Ptr_St;

typedef struct s64Vector2_s {
    s64_t x, y;
} s64Vector2_St, *s64Vector2Ptr_St;

typedef struct f32Vector2_s {
    f32_t x, y;
} f32Vector2_St, *f32Vector2Ptr_St;

typedef struct f64Vector2_s {
    f64_t x, y;
} f64Vector2_St, *f64Vector2Ptr_St;


typedef enum CellStates_e {
    EMPTY = 0, 
    PLAYER, 
    COMPUTER
} CellState_Et;

typedef CellState_Et Board_t[BOARD_SIZE][BOARD_SIZE];

typedef struct ValidCell_s {
    s64Vector2_St pos;
    u64_t score;
} ValidMove_St;

typedef struct AnimPiece_s {
    s64Vector2_St pos;
    enum { ANIM_NONE, ANIM_PLACE, ANIM_FLIP } type;
    float timer;
    float duration;  // e.g., 0.5f
    CellState_Et fromState;  // for flip color change
    CellState_Et toState;
} AnimPiece_St;

#endif // TYPES_H