#ifndef RENDERING_H
#define RENDERING_H

#include "types.h"

void drawPieces(Board_t board);
void drawPiece(Piece_st* piece);

void drawCell(int x, int y, Color c);
void drawCheckerboard(void);
void drawBorder(void);

void drawPositionsPossibles(Board_t board);

void printPromotion(void);

void renderFrame(Board_t board);

#endif