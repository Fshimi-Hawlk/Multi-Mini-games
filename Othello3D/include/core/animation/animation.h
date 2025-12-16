#ifndef ANIMATION_H
#define ANIMATION_H

#include "utils/types.h"

void initAnimation(Board_t board, s64Vector2_St initPos, CellState_Et toState, s64Vector2_St flipped[], u64_t flippedCount);

bool advanceAnimation(Board_t board);

#endif