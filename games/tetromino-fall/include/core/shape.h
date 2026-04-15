/**
    @file shape.h
    @author Fshimi-Hawlk
    @date 2026-02-06
    @date 2026-04-14
    @brief Core logic for tetromino shapes and their transformations.
*/
#ifndef CORE_SHAPE_H
#define CORE_SHAPE_H

#include "utils/types.h"

#define I_SHAPE { \
    {-1, 0}, {0, 0}, {1, 0}, {2, 0} \
}

#define O_SHAPE { \
    {0, 0}, {1, 0}, \
    {0, 1}, {1, 1} \
}

#define T_SHAPE { \
    {-1, 0}, {0, 0}, {1, 0},  \
             {0, 1} \
}

#define S_SHAPE { \
             {0, 0}, {1, 0},  \
    {-1, 1}, {0, 1} \
}

#define Z_SHAPE { \
    {-1, 0}, {0, 0},  \
             {0, 1}, {1, 1} \
}

#define J_SHAPE { \
             {0, -1}, \
             {0, 0}, \
    {-1, 1}, {0, 1} \
}

#define L_SHAPE { \
    {0, -1}, \
    {0, 0}, \
    {0, 1}, {1, 1} \
}

/**
    @brief Initializes a board shape with a random tetromino.

    @param[out]    boardShape   The shape to initialize.
*/
void tetrominoFall_randomShape(BoardShape_St* boardShape);

/**
    @brief Rotates the given shape clockwise.

    @param[in,out] boardShape   The shape to rotate.
*/
void tetrominoFall_rotationCW(BoardShape_St* boardShape);

/**
    @brief Rotates the given shape counter-clockwise.

    @param[in,out] boardShape   The shape to rotate.
*/
void tetrominoFall_rotationCCW(BoardShape_St* boardShape);

/**
    @brief Updates the shape position based on time for automatic dropping.

    @param[in,out] speed        The speed state controlling the drop interval.
    @param[in,out] boardShape   The shape to drop.
    @param[in]     dt           The delta time between each frames.
*/
void tetrominoFall_automaticDrop(Speed_St* speed, BoardShape_St* boardShape, float dt);

#endif // CORE_SHAPE_H
