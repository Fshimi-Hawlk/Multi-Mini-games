#ifndef CORE_GAME_H
#define CORE_GAME_H

#include "utils/types.h"
#include "utils/globals.h"

void automaticMovementTo(speed_st* speed, boardShape_st* boardShape, moveAlgoResult_st targetMove);
void mouvement(board_t board, boardShape_st* boardShape);

void readHighScore(int *highScore);
void writeHighScore(int highScore, int score);

#endif