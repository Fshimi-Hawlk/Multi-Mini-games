/**
 * @file chessAPI.h
 * @brief Public interface for the Chess mini-game.
 */

#ifndef CHESS_API_H
#define CHESS_API_H

#include "APIs/generalAPI.h"
#include "networkInterface.h"

enum ChessActionCodes_e {
    ACTION_CODE_CHESS_MOVE = firstAvailableActionCode + 0x10,
    ACTION_CODE_CHESS_SYNC
};

typedef struct {
    BaseGame_St base;
} ChessGame_St;

Error_Et chess_initGame(ChessGame_St** game);
Error_Et chess_freeGame(ChessGame_St** game);

#endif
