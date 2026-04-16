/**
    @file shape.c
    @author Fshimi-Hawlk
    @date 2026-02-06
    @date 2026-04-14
    @brief Core implementation of tetromino shapes and their transformations.
*/

#include "core/shape.h"
#include "utils/globals.h"

void tetrominoFall_randomShape(BoardShape_St* boardShape) {
    int n = rand() % SHAPE_MAX_ID;
    memcpy(boardShape->shape, tetraminosShapes[n], sizeof(Tetromino_t));
    boardShape->color = tetraminosColors[n];
    boardShape->position = (iVector2){4, 0};
    boardShape->shapeName = n;
}

void tetrominoFall_rotationCW(BoardShape_St* boardShape) {
    int xTemp;
    
    for (int i = 0; i < 4; i++) {
        xTemp = boardShape->shape[i].x;
        boardShape->shape[i].x = -boardShape->shape[i].y;
        boardShape->shape[i].y = xTemp;
    }

    boardShape->rotation = (boardShape->rotation + 1) % 4;
}

void tetrominoFall_rotationCCW(BoardShape_St* boardShape) {
    int xTemp;
    
    for (int i = 0; i < 4; i++) {
        xTemp = boardShape->shape[i].x;
        boardShape->shape[i].x = boardShape->shape[i].y;
        boardShape->shape[i].y = -xTemp;
    }

    boardShape->rotation = (boardShape->rotation + 3) % 4;
}

void tetrominoFall_automaticDrop(Speed_St* speed, BoardShape_St* boardShape, float dt) {
    speed->t += dt;
    speed->tDrop = fminf(speed->t / speed->duration, 1.0f);
    if (speed->tDrop >= 1) {
        speed->t = 0.0f;
        speed->tDrop = 0.0f;
        (boardShape->position.y)++;
    }
}
