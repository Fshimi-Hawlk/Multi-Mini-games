#ifndef CORE_ALGO_H
#define CORE_ALGO_H

#include "utils/types.h"

void copyBoard(board_t src, board_t dest);
int evaluateBoard(board_t board);
int simulateDrop(board_t board, boardShape_st piece, int col);
moveAlgoResult_st findBestMove(board_t board, boardShape_st shape, boardShape_st nextShape);

#endif