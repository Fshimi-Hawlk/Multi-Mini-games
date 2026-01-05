#ifndef UTILS_H
#define UTILS_H

#include "types.h"

#define CBCGC(x, y) (x) + 'a', BOARD_SIZE - (y)

#define isOOB(col, row) ((col < 0 || col >= BOARD_SIZE || row < 0 || row >= BOARD_SIZE))

void printMovesMade(void);

IVec2_st GetMousePositionI(void);

#endif