/**
    @file shape.h
    @author Léandre BAUDET
    @date 2026-04-14
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
void randomShape(boardShape_st* boardShape);

/**
    @brief Rotates the given shape clockwise.

    @param[in,out] boardShape   The shape to rotate.
*/
void rotationCW(boardShape_st* boardShape);

/**
    @brief Rotates the given shape counter-clockwise.

    @param[in,out] boardShape   The shape to rotate.
*/
void rotationCCW(boardShape_st* boardShape);

/**
    @brief Updates the shape position based on time for automatic dropping.

    @param[in,out] speed        The speed state controlling the drop interval.
    @param[in,out] boardShape   The shape to drop.
*/
void automaticDrop(speed_st* speed, boardShape_st* boardShape);

#endif // CORE_SHAPE_H
