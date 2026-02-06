#include "core/shape.h"

void randomShape(boardShape_st* boardShape) {
    int n = rand() % SHAPE_MAX_ID;
    memcpy(boardShape->shape, tetraminosShapes[n], sizeof(tetramino));
    boardShape->color = tetraminosColors[n];
    boardShape->position = (iVector2){4, 0};
    boardShape->shapeName = n;
}

void rotationCW(boardShape_st* boardShape) {
    int xTemp;
    
    for (int i = 0; i < 4; i++) {
        xTemp = boardShape->shape[i].x;
        boardShape->shape[i].x = -boardShape->shape[i].y;
        boardShape->shape[i].y = xTemp;
    }

    boardShape->rotation = (boardShape->rotation + 1) % 4;
}

void rotationCCW(boardShape_st* boardShape) {
    int xTemp;
    
    for (int i = 0; i < 4; i++) {
        xTemp = boardShape->shape[i].x;
        boardShape->shape[i].x = boardShape->shape[i].y;
        boardShape->shape[i].y = -xTemp;
    }

    boardShape->rotation = (boardShape->rotation + 3) % 4;
}

void automaticDrop(speed_st* speed, boardShape_st* boardShape) {
    speed->t += GetFrameTime();
    speed->tDrop = fminf(speed->t / speed->duration, 1.0f);
    if (speed->tDrop >= 1) {
        speed->t = 0.0f;
        speed->tDrop = 0.0f;
        (boardShape->position.y)++;
    }
}
