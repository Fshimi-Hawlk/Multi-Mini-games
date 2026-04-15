/**
    @file types.h
    @author Fshimi-Hawlk
    @date 2026-02-06
    @date 2026-04-14
    @brief Type definitions for the Tetromino Fall game, including shapes, board, and game state structures.
*/
#ifndef UTILS_TYPES_H
#define UTILS_TYPES_H

#include "common.h"

/**
    @brief Array of 4 2D vectors representing the relative positions of blocks in a tetramino.
*/
typedef iVector2 tetramino[4];
/**
    @brief 2D array of colors representing the game board state.
*/
typedef Color board_t[BOARD_HEIGHT][BOARD_WIDTH];


/**
    @brief Identifiers for each of the seven standard tetramino shapes.
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
    @brief State of an active tetramino on the board, including its shape, position, and orientation.
*/
typedef struct {
    tetramino shape;
    iVector2 position;
    Color color;
    int rotation;
    int shapeName;
} boardShape_st;

/**
    @brief Timing and speed parameters for the game's automatic drop mechanism.
*/
typedef struct {
    float t;
    float tDrop;
    float duration;
} speed_st;

/**
    @brief Configuration for input repeat delay and frequency (DAS - Delayed Auto Shift).
*/
typedef struct {
    float leftTimer;
    float rightTimer;
    float downTimer;
    float initialDelay;
    float repeatDelay;
} inputRepeat_st;

/**
    @brief Result of the move-finding algorithm, specifying target position and rotation.
*/
typedef struct {
    iVector2 position;
    int rotation;
} moveAlgoResult_st;

#endif
