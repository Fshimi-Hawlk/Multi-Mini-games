#ifndef CORE_BOARD_H
#define CORE_BOARD_H

#include "utils/types.h"

void initBoard(board_t board);

bool areCoordinatesOOB(int x, int y);
bool isPositonOOB(iVector2 position);
bool isOOBAt(boardShape_st boardShape, iVector2 position);
bool isOOB(boardShape_st boardShape);
bool isCollidingAt(board_t board, boardShape_st boardShape, iVector2 position);
bool isColliding(board_t board, boardShape_st boardShape);

void putShapeInBoard(board_t board, boardShape_st boardShape);

void detectFullLines(board_t board, int lineArray[4], int *lineNb);
void clearLines(board_t board, int lineArray[4], int lineNb);
void handleLineClears(board_t board, int lineArray[4], int *lineNb);

#endif