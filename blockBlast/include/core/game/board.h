#ifndef CORE_GAME_BOARD_H
#define CORE_GAME_BOARD_H

#include "utils/userTypes.h"

bool8 isInBound(const s8Vector2 pos);

bool8 checkBoardForClearing(const Board_St* const board);
void clearBoard(Board_St* const board);

#endif // CORE_GAME_BOARD_H