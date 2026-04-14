/**
    @file ai.h
    @author Léandre BAUDET
    @date 2024-01-01
    @date 2026-04-14
    @brief Chess AI implementation using Minimax and Alpha-Beta pruning.
*/
#ifndef CHESS_AI_H
#define CHESS_AI_H

#include "types.h"
#include "baseTypes.h"

/**
    @brief Represents a single chess move with its associated score.
*/
typedef struct {
    IVec2_st from;  ///< Starting position
    IVec2_st to;    ///< Ending position
    s32 score;      ///< Move score for evaluation
} ChessMove_st;

/**
    @brief Computes the best move for the current player using Minimax with Alpha-Beta pruning.
    @param[in]     board    The current game board.
    @param[in,out] white    Player structure for White.
    @param[in,out] black    Player structure for Black.
    @param[in]     player   The player to move (0: White, 1: Black).
    @param[in]     depth    The search depth.
    @return                 The best move found.
*/
ChessMove_st ai_getBestMove(Board_t board, Player_st* white, Player_st* black, s32 player, u8 depth);

/**
    @brief Evaluates the board from the perspective of the White player.
    @param[in]     board    The game board.
    @return                 Evaluation score (s32).
*/
s32 ai_evaluateBoard(Board_t board);

#endif // CHESS_AI_H
