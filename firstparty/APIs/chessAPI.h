/**
    @file chessAPI.h
    @author Léandre BAUDET
    @date 2024-01-01
    @brief Public interface for the chess mini-game.
*/
#ifndef CHESS_API_H
#define CHESS_API_H

#include "APIs/generalAPI.h"
#include "networkInterface.h"

/**
    @brief Action codes for chess-specific network messages.
*/
enum ChessActionCodes_e {
    ACTION_CODE_CHESS_MOVE = firstAvailableActionCode + 0x10, ///< Move piece action
    ACTION_CODE_CHESS_SYNC                                    ///< Sync game state action
};

/**
    @brief Main structure representing the chess game state.
*/
typedef struct {
    BaseGame_St base; ///< Base game structure
} ChessGame_St;

/**
    @brief Initializes the chess game.
    @param[out] game  Pointer to the chess game structure to initialize
    @return           Error_Et code indicating success or failure
*/
Error_Et chess_initGame(ChessGame_St** game);

/**
    @brief Frees the chess game resources.
    @param[in,out] game  Pointer to the chess game structure to free
    @return              Error_Et code indicating success or failure
*/
Error_Et chess_freeGame(ChessGame_St** game);

#endif