#ifndef CORE_GAME_BOTALGO_H
#define CORE_GAME_BOTALGO_H

#include "utils/types.h"

/**
 * @brief Checks if position is next to a corner.
 * @param pos Position to check.
 * @return true if adjacent to corner.
 */
bool isAdjacentToCorner(s64Vector2_St pos);

/**
 * @brief Calculates heuristic score for a move.
 * @param board The game board.
 * @param pos Position to score.
 * @param playerTurn true for player, false for computer.
 * @return Score value.
 */
u64_t computeMoveScore(const Board_t board, s64Vector2_St pos, bool playerTurn);

/**
 * @brief Finds index of highest scored move.
 * @param moves Array of valid moves.
 * @param count Number of moves.
 * @return Index of best move.
 */
u64_t findBestMoveIndex(const ValidMove_St moves[], u64_t count);


#endif // CORE_GAME_BOTALGO_H