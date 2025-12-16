#ifndef CORE_GAME_BOARD_H
#define CORE_GAME_BOARD_H

#include "utils/types.h"

/**
 * @brief Initializes the game board with starting positions.
 * @param board The board array to initialize.
 */
void initBoard(Board_t board);

/**
 * @brief Places piece and flips opponents.
 * @param board The game board.
 * @param pos Position to place.
 * @param playerTurn true for player, false for computer.
 */
void placePieceAndFlip(Board_t board, s64Vector2_St pos, bool playerTurn);

/**
 * @brief Flips pieces in one direction.
 * @param board The game board.
 * @param pos Starting position.
 * @param dir Direction to flip.
 * @param playerTurn true for player, false for computer.
 */
void flipInDirection(Board_t board, s64Vector2_St pos, s64Vector2_St dir, bool playerTurn);

/**
 * @brief Handles one turn cycle.
 * @param board The game board.
 * @param mousePos Pointer to mouse vector.
 * @param clickedPos Pointer to clicked board pos.
 * @param lastMove Pointer to last move pos.
 * @param flipped Array for flipped.
 * @param flippedCount Pointer to count.
 * @return true if game ended.
 */
bool processTurn(Board_t board,
                 s64Vector2_St clickedPos,
                 s64Vector2_St* lastMove,
                 s64Vector2_St flipped[], u64_t* flippedCount);

#endif // CORE_GAME_BOARD_H