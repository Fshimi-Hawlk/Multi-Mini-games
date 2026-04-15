/**
    @file gameRegistry.h
    @author Multi Mini-Games Team
    @date 2026-04-07
    @date 2026-04-14
    @brief Unified static registry for server-side game modules.
*/
#ifndef GAME_REGISTRY_H
#define GAME_REGISTRY_H

#include "networkInterface.h"
#include "APIs/generalAPI.h"

// Uniform naming convention for all game module interfaces
extern GameServerInterface_St lobbyServerInterface;
/**
    @brief Global variable bingoServerInterface
*/
extern GameServerInterface_St bingoServerInterface;
/**
    @brief Global variable chessServerInterface
*/
extern GameServerInterface_St chessServerInterface;
/**
    @brief Global variable kingServerInterface
*/
extern GameServerInterface_St kingServerInterface;
/**
    @brief Global variable rubikServerInterface
*/
extern GameServerInterface_St twistCubeServerInterface;

/**
 * @brief Get the server interface for a given game ID.
 * @param gameId The game identifier (MiniGame_Et).
 * @return A pointer to the interface, or NULL if not found.
 */
const GameServerInterface_St* getGameServerInterface(MiniGameId_Et gameId);

#endif // GAME_REGISTRY_H
