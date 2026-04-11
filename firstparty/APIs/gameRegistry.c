/**
 * @file gameRegistry.c
 * @author i-Charlys
 * @date 2026-04-07
 * @brief Implementation of the unified static registry for server-side game modules.
 */

#include "gameRegistry.h"
#include <stddef.h>

static const GameServerInterface_St* gameInterfaces[__miniGameCount] = {
    [MINI_GAME_LOBBY]   = &lobbyServerInterface,
    [MINI_GAME_KFF]     = &kingServerInterface,
    [MINI_GAME_CHESS]   = &chessServerInterface,
    [MINI_GAME_CUBE]    = &rubikServerInterface,
    [MINI_GAME_BINGO]   = &bingoServerInterface,
};

const GameServerInterface_St* getGameServerInterface(MiniGame_Et gameId) {
    if (gameId >= 0 && gameId < __miniGameCount) {
        return gameInterfaces[gameId];
    }
    return NULL;
}
