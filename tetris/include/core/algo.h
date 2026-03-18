#ifndef CORE_ALGO_H
#define CORE_ALGO_H

#include "utils/types.h"

void tetris_copyBoard(board_t src, board_t dest);
int tetris_evaluateBoard(board_t board);
int tetris_simulateDrop(board_t board, boardShape_st piece, int col);
moveAlgoResult_st tetris_findBestMove(board_t board, boardShape_st shape, boardShape_st nextShape);

#endif