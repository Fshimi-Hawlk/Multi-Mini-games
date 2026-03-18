#ifndef CORE_BOARD_H
#define CORE_BOARD_H

#include "utils/types.h"

void tetris_initBoard(board_t board);

bool tetris_areCoordinatesOOB(int x, int y);
bool tetris_isPosOOB(iVector2 position);
bool tetris_tetris_isOOBAt(boardShape_st boardShape, iVector2 position);
bool tetris_isOOB(boardShape_st boardShape);
bool tetris_tetris_isCollidingAt(board_t board, boardShape_st boardShape, iVector2 position);
bool tetris_isColliding(board_t board, boardShape_st boardShape);

void tetris_putShapeInBoard(board_t board, boardShape_st boardShape);

void tetris_detectFullLines(board_t board, int lineArray[4], int *lineNb);
void tetris_clearLines(board_t board, int lineArray[4], int lineNb);
void tetris_handleLineClears(board_t board, int lineArray[4], int *lineNb);

#endif