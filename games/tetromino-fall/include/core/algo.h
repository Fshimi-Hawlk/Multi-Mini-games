/**
    @file algo.h
    @author Fshimi-Hawlk
    @date 2026-02-06
    @date 2026-04-14
    @brief AI algorithms for Tetris move optimization and board evaluation.
*/

#ifndef CORE_ALGO_H
#define CORE_ALGO_H

#include "utils/types.h"

/**
    @brief Copies the content of one board to another.

    @param[in]  src   Source board to copy from.
    @param[out] dest  Destination board to copy to.
*/
void tetrominoFall_copyBoard(Board_t src, Board_t dest);

/**
    @brief Evaluates the current state of the board and returns a score.

    The score is calculated based on holes, aggregate height, bumpiness,
    max height, and complete lines. Higher is better.

    @param[in] board  The board to evaluate.
    @return           The calculated score for the board state.
*/
int tetrominoFall_evaluateBoard(Board_t board);

/**
    @brief Simulates dropping a piece in a specific column.

    @param[in] board  The current board state.
    @param[in] piece  The piece to drop.
    @param[in] col    The column index to drop the piece in.
    @return           The resulting Y position of the piece after dropping.
*/
int tetrominoFall_simulateDrop(Board_t board, BoardShape_St piece, int col);

/**
    @brief Finds the best move for the current and next shape.

    Uses a look-ahead of one piece to evaluate all possible rotations and columns.

    @param[in] board      The current board state.
    @param[in] shape      The current shape to place.
    @param[in] nextShape   The next shape that will appear.
    @return               A struct containing the best position and rotation.
*/
MoveAlgoResult_St tetrominoFall_findBestMove(Board_t board, BoardShape_St shape, BoardShape_St nextShape);

#endif // CORE_ALGO_H
