/**
    @file shape.c
    @author Léandre BAUDET
    @date 2026-04-14
    @date 2026-04-14
    @brief Core implementation of tetromino shapes and their transformations.
*/
#include "core/shape.h"
#include "utils/globals.h"

/**
    @brief Initializes a board shape with a random tetromino.

    @param[out]    boardShape   The shape to initialize.
*/
void randomShape(boardShape_st* boardShape) {
    int n = rand() % SHAPE_MAX_ID;
    memcpy(boardShape->shape, tetraminosShapes[n], sizeof(tetramino));
    boardShape->color = tetraminosColors[n];
    boardShape->position = (iVector2){4, 0};
    boardShape->shapeName = n;
}

/**
    @brief Rotates the given shape clockwise.

    @param[in,out] boardShape   The shape to rotate.
*/
void rotationCW(boardShape_st* boardShape) {
    int xTemp;
    
    for (int i = 0; i < 4; i++) {
        xTemp = boardShape->shape[i].x;
        boardShape->shape[i].x = -boardShape->shape[i].y;
        boardShape->shape[i].y = xTemp;
    }

    boardShape->rotation = (boardShape->rotation + 1) % 4;
}

/**
    @brief Rotates the given shape counter-clockwise.

    @param[in,out] boardShape   The shape to rotate.
*/
void rotationCCW(boardShape_st* boardShape) {
    int xTemp;
    
    for (int i = 0; i < 4; i++) {
        xTemp = boardShape->shape[i].x;
        boardShape->shape[i].x = boardShape->shape[i].y;
        boardShape->shape[i].y = -xTemp;
    }

    boardShape->rotation = (boardShape->rotation + 3) % 4;
}

/**
    @brief Updates the shape position based on time for automatic dropping.

    @param[in,out] speed        The speed state controlling the drop interval.
    @param[in,out] boardShape   The shape to drop.
*/
void automaticDrop(speed_st* speed, boardShape_st* boardShape) {
    speed->t += GetFrameTime();
    speed->tDrop = fminf(speed->t / speed->duration, 1.0f);
    if (speed->tDrop >= 1) {
        speed->t = 0.0f;
        speed->tDrop = 0.0f;
        (boardShape->position.y)++;
    }
}
