#ifndef UI_GAME_H
#define UI_GAME_H

#include "utils/types.h"

void tetris_drawInformations(int score, int level, int lineNbTotal, int highScore);
void tetris_drawPreview(board_t board, boardShape_st boardShape);

#endif