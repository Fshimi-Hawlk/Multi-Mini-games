#ifndef CORE_SHAPE_H
#define CORE_SHAPE_H

#include "utils/types.h"
#include "utils/globals.h"

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

void randomShape(boardShape_st* boardShape);
void rotationCW(boardShape_st* boardShape);
void rotationCCW(boardShape_st* boardShape);
void automaticDrop(speed_st* speed, boardShape_st* boardShape);

#endif