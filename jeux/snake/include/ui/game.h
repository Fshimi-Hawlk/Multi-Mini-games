#ifndef UI_GAME_H
#define UI_GAME_H

#include "utils/userTypes.h"

void snake_drawBoard(const Board_t board);
void snake_drawSnake(const Snake_St* const snake, f32 interpolation, iVector2 direction);

#endif // UI_GAME_H