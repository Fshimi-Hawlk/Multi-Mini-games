#ifndef UI_GAME_H
#define UI_GAME_H

#include "utils/types.h"
#include "core/board.h"

void drawInformations(int score, int level, int lineNbTotal, int highScore);
void drawPreview(board_t board, boardShape_st boardShape);

#endif