#ifndef ALGO_H
#define ALGO_H

#include "types.h"

void selectPiece(Board_t board, IVec2_st targetPos);

bool movement(Board_t board, IVec2_st boardPos);

bool canBePlaced(Board_t board, int col, int lig);
bool isInCheck(Board_t board, Piece_st* selectionnedPiece, int col, int lig, int joueur);
bool isCheckmate(Board_t board);
bool isCheckmate(Board_t board);
bool isSquareThreatened(Board_t board, int joueurM, int xCase, int yCase);

int updatePossibleMoves(Board_t board);

void promotionChoice(Board_t board);

void promotePiece(Piece_st* pion, PieceName_et promotionVers, const char **couleurChemin);

void applyPredifinedMoves(Board_t board, char *coupPredefinis[], int nCoup);

#endif