#ifndef CHESS_AI_H
#define CHESS_AI_H

#include "types.h"
#include "baseTypes.h"

typedef struct {
    IVec2_st from;
    IVec2_st to;
    s32 score;
} ChessMove_st;

/**
 * @brief Computes the best move for the current player using Minimax with Alpha-Beta pruning.
 * @param board The current game board.
 * @param white Player structure for White.
 * @param black Player structure for Black.
 * @param player The player to move (0: White, 1: Black).
 * @param depth The search depth.
 * @return The best move found.
 */
ChessMove_st ai_getBestMove(Board_t board, Player_st* white, Player_st* black, s32 player, u8 depth);

/**
 * @brief Evaluates the board from the perspective of the White player.
 * @param board The game board.
 * @return Evaluation score (s32).
 */
s32 ai_evaluateBoard(Board_t board);

#endif // CHESS_AI_H
