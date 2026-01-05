#ifndef ALGO_H
#define ALGO_H

#include "types.h"

void selectPiece(Board_t board, IVec2_st targetPos);

bool movement(Board_t board, Piece_st* selectionnedPiece, IVec2_st boardPos);

bool canBePlaced(Board_t board, Piece_st* selectionnedPiece, int col, int row);
bool isInCheck(Board_t board, Piece_st* selectionnedPiece, int col, int lig, int joueur);
bool isCheckmate(Board_t board);
bool isStalemate(Board_t board);
bool isSquareThreatened(Board_t board, int joueurM, int xCase, int yCase);

void updatePossibleMoves(Board_t board);

void promotionChoice(Board_t board);

void applyPredifinedMoves(Board_t board, char *coupPredefinis[], int nCoup);

#endif