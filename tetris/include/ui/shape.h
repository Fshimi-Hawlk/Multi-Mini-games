/**
    @file ui/shape.h
    @brief UI rendering functions for Tetris shapes/pieces.
 */

#ifndef UI_SHAPE_H
#define UI_SHAPE_H

#include "utils/types.h"

void drawShape(boardShape_st boardShape);
void drawNextShape(boardShape_st boardShape);

#endif