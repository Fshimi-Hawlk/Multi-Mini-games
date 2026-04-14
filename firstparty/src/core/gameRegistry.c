/**
    @file gameRegistry.c
    @author Multi Mini-Games Team
    @date 2026-04-07
    @date 2026-04-14
    @brief Implementation of the unified static registry for server-side game modules.
*/
#include "gameRegistry.h"

/*
    @brief Array of the multi-player game server interfaces.
    @note The solo-player game doesn't implement a server interface, so they will always be NULL.
*/
static const GameServerInterface_St* gameInterfaces[__miniGameIdCount] = {
    [MINI_GAME_ID_BINGO]            = &bingoServerInterface,
    [MINI_GAME_ID_BOWLING]          = NULL,
    [MINI_GAME_ID_CHESS]            = &chessServerInterface,
    [MINI_GAME_ID_DISC_REVERSAL]    = NULL, // TODO
    [MINI_GAME_ID_DROP_FOUR]        = NULL, // TODO
    [MINI_GAME_ID_EDITOR]           = NULL,
    [MINI_GAME_ID_KING_FOR_FOUR]    = &kingServerInterface,
    [MINI_GAME_ID_LOBBY]            = &lobbyServerInterface,
    [MINI_GAME_ID_MINI_GOLF]        = NULL,
    [MINI_GAME_ID_POLY_BLAST]       = NULL,
    [MINI_GAME_ID_SNAKE]            = NULL,
    [MINI_GAME_ID_SOLO_CARDS]       = NULL,
    [MINI_GAME_ID_SUIKA]            = NULL,
    [MINI_GAME_ID_TETROMINO_FALL]   = NULL,
    [MINI_GAME_ID_TWIST_CUBE]       = &rubikServerInterface,
};

const GameServerInterface_St* getGameServerInterface(MiniGameId_Et gameId) {
    return (0 <= gameId && gameId < __miniGameIdCount)
           ? gameInterfaces[gameId]
           : NULL;
}
