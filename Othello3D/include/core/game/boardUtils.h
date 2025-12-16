#ifndef CORE_GAME_BOARDUTILS_H
#define CORE_GAME_BOARDUTILS_H

#include "utils/types.h"

/**
 * @brief Checks if the board has no empty cells left.
 * @param board The board to check.
 * @return true if full, false otherwise.
 */
bool isBoardFull(const Board_t board);

/**
 * @brief Converts screen mouse position to board coordinates.
 * @param mousePos Mouse position vector.
 * @return Board position or {-1, -1} if invalid.
 */
s64Vector2_St mouseToBoardPos(Vector2 mousePos);

/**
 * @brief Checks if position is outside board bounds.
 * @param pos Position to check.
 * @return true if out of bounds, false otherwise.
 */
bool isOutOfBounds(s64Vector2_St pos);

/**
 * @brief Validates if a direction allows flipping opponents.
 * @param board The game board.
 * @param pos Placement position.
 * @param dir Direction vector.
 * @param playerTurn true for player, false for computer.
 * @return true if valid bracket in direction.
 */
bool isDirectionValid(const Board_t board, s64Vector2_St pos, s64Vector2_St dir, bool playerTurn);

/**
 * @brief Checks if move at position is legal.
 * @param board The game board.
 * @param pos Position to place.
 * @param playerTurn true for player, false for computer.
 * @return true if valid move.
 */
bool isValidMove(const Board_t board, s64Vector2_St pos, bool playerTurn);

/**
 * @brief Checks if the turn holder has any legal moves.
 * @param board The game board.
 * @param playerTurn true for player, false for computer.
 * @return true if has moves.
 */
bool hasValidMoves(const Board_t board, bool playerTurn);

/**
 * @brief Counts pieces for each side.
 * @param board The game board.
 * @param player Pointer to player count.
 * @param computer Pointer to computer count.
 */
void countPieces(const Board_t board, u64_t* player, u64_t* computer);

/**
 * @brief Counts flips in one direction.
 * @param board The game board.
 * @param pos Starting position.
 * @param dir Direction.
 * @param playerTurn true for player, false for computer.
 * @return Number of flips.
 */
u64_t countFlipsInDirection(const Board_t board, s64Vector2_St pos, s64Vector2_St dir, bool playerTurn);

/**
 * @brief Sums flips across all directions.
 * @param board The game board.
 * @param pos Position.
 * @param playerTurn true for player, false for computer.
 * @return Total flips.
 */
u64_t countTotalFlips(const Board_t board, s64Vector2_St pos, bool playerTurn);

/**
 * @brief Collects positions of flipped pieces.
 * @param board The game board.
 * @param pos Placement position.
 * @param playerTurn true for player, false for computer.
 * @param outList Array for flipped positions.
 * @param outCount Pointer to count.
 */
void collectFlippedPieces(const Board_t board, s64Vector2_St pos, bool playerTurn,
                         s64Vector2_St outList[], u64_t* outCount);

/**
 * @brief Converts screen mouse position + ray to board coordinates using raycast.
 *
 * @param mousePos Mouse position.
 * @param cam Current camera.
 * @return Board position or {-1,-1} if invalid.
 */
s64Vector2_St getBoardCellFromRay(Vector2 mousePos, Camera3D cam);

#endif // CORE_GAME_BOARDUTILS_H