#ifndef CORE_GAME_H
#define CORE_GAME_H

#include "utils/types.h"
#include "utils/globals.h"

void tetris_automaticMovementTo(speed_st* speed, boardShape_st* boardShape, moveAlgoResult_st targetMove);
void tetris_mouvement(board_t board, boardShape_st* boardShape);

void tetris_readHighScore(int *highScore);
void tetris_writeHighScore(int highScore, int score);

#endif