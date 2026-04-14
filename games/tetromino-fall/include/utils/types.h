/**
    @file types.h
    @author Léandre BAUDET
    @date 2026-04-14
    @date 2026-04-14
    @brief types.h implementation/header file
*/
#ifndef UTILS_TYPES_H
#define UTILS_TYPES_H

#include "common.h"

/**
    @brief Global variable tetramino[4]
*/
typedef iVector2 tetramino[4];
/**
    @brief Global variable board_t[BOARD_HEIGHT][BOARD_WIDTH]
*/
typedef Color board_t[BOARD_HEIGHT][BOARD_WIDTH];


/**
    @brief Definition of typedef enum
*/
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

/**
    @brief Definition of typedef struct
*/
typedef struct {
    tetramino shape;
    iVector2 position;
    Color color;
    int rotation;
    int shapeName;
} boardShape_st;

/**
    @brief Definition of typedef struct
*/
typedef struct {
    float t;
    float tDrop;
    float duration;
} speed_st;

/**
    @brief Definition of typedef struct
*/
typedef struct {
    float leftTimer;
    float rightTimer;
    float downTimer;
    float initialDelay;
    float repeatDelay;
} inputRepeat_st;

/**
    @brief Definition of typedef struct
*/
typedef struct {
    iVector2 position;
    int rotation;
} moveAlgoResult_st;

#endif