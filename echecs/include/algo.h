/**
 * @file algo.h
 * @author Maxime CHAUVEAU
 * @brief Game logic and algorithms for Echecs.
 * @version 1.0
 * @date 2024
 *
 * This file contains all the game logic functions including
 * piece movement, move validation, check/checkmate detection,
 * and promotion handling.
 */

#ifndef ALGO_H
#define ALGO_H

#include "types.h"

/**
 * @brief Select a piece on the board.
 * @param board The game board
 * @param targetPos The position of the piece to select
 */
void selectPiece(Board_t board, IVec2_st targetPos);

/**
 * @brief Attempt to move a piece to a new position.
 * @param board The game board
 * @param selectionnedPiece The piece to move
 * @param boardPos The target position
 * @return true if the move was successful, false otherwise
 */
bool movement(Board_t board, Piece_st* selectionnedPiece, IVec2_st boardPos);

/**
 * @brief Check if a piece can be placed at a specific position.
 * @param board The game board
 * @param selectionnedPiece The piece to check
 * @param col The target column
 * @param row The target row
 * @return true if the move is valid, false otherwise
 */
bool canBePlaced(Board_t board, Piece_st* selectionnedPiece, int col, int row);

/**
 * @brief Check if a move would leave the king in check.
 * @param board The game board
 * @param selectionnedPiece The piece to move
 * @param col The target column
 * @param lig The target row
 * @param joueur The player whose turn it is (0 for white, 1 for black)
 * @return true if the move would result in check, false otherwise
 */
bool isInCheck(Board_t board, Piece_st* selectionnedPiece, int col, int lig, int joueur);

/**
 * @brief Check if the current player is in checkmate.
 * @param board The game board
 * @return true if checkmate, false otherwise
 */
bool isCheckmate(Board_t board);

/**
 * @brief Check if the current player is in stalemate.
 * @param board The game board
 * @return true if stalemate, false otherwise
 */
bool isStalemate(Board_t board);

/**
 * @brief Check if a square is threatened by opponent pieces.
 * @param board The game board
 * @param playerM The player to check (0 for white, 1 for black)
 * @param xCase The x coordinate of the square
 * @param yCase The y coordinate of the square
 * @return true if the square is threatened, false otherwise
 */
bool isSquareThreatened(Board_t board, int playerM, int xCase, int yCase);

/**
 * @brief Update the list of possible moves for the selected piece.
 * @param board The game board
 */
void updatePossibleMoves(Board_t board);

/**
 * @brief Handle pawn promotion choice.
 * @param board The game board
 */
void promotionChoice(Board_t board);

/**
 * @brief Apply a series of predefined moves (for demo/simulation).
 * @param board The game board
 * @param coupPredefinis Array of move strings in algebraic notation
 * @param nCoup Number of moves to apply
 */
void applyPredifinedMoves(Board_t board, char *coupPredefinis[], int nCoup);

#endif