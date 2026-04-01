/**
 * @file chessAPI.h
 * @brief Public interface for the Chess mini-game.
 */

#ifndef CHESS_API_H
#define CHESS_API_H

#include "APIs/generalAPI.h"

typedef struct {
    BaseGame_St base;
} ChessGame_St;

Error_Et chess_initGame(ChessGame_St** game);
Error_Et chess_freeGame(ChessGame_St** game);

#endif
