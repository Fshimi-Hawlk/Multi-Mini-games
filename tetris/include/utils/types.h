#ifndef UTILS_TYPES_H
#define UTILS_TYPES_H

#include "configs.h"
#include "common.h"

typedef struct iVector2 {
    int x, y;
} iVector2;

typedef iVector2 tetramino[4];
typedef Color board_t[BOARD_HEIGHT][BOARD_WIDTH];


typedef enum {
    I_SHAPE_ID, 
    O_SHAPE_ID, 
    T_SHAPE_ID, 
    S_SHAPE_ID, 
    Z_SHAPE_ID, 
    J_SHAPE_ID, 
    L_SHAPE_ID,
    SHAPE_MAX_ID
} shapeId;

typedef struct {
    tetramino shape;
    iVector2 position;
    Color color;
    int rotation;
    int shapeName;
} boardShape_st;

typedef struct {
    float t;
    float tDrop;
    float duration;
} speed_st;

typedef struct {
    float leftTimer;
    float rightTimer;
    float downTimer;
    float initialDelay;
    float repeatDelay;
} inputRepeat_st;

typedef struct {
    iVector2 position;
    int rotation;
} moveAlgoResult_st;

#endif