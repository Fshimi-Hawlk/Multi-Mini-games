#ifndef GAME_H
#define GAME_H

#include "types.h"

int initPlayers(void);

Player_st* initPlayer(ColorPiece_et color, int mainLineY, int pawnLineY);

int initTextures(void);
void freeTextures(void);

int initBoard(Board_t board);

int initGame(Board_t board);

void resetGame();

void gameLoop(Board_t board, char *predifinedMoves[], int nbMoves);

void freePlayer(Player_st* player);

void freeGame(void);

#endif