/**
    @file gameNameAPI.h
    @author Multi Mini-Games Team
    @date 2026-02-07
    @date 2026-04-14
    @brief Public API for the GameName mini-game.
*/
#ifndef GAME_NAME_API_H
#define GAME_NAME_API_H

#include "baseTypes.h"
#include "networkInterface.h"

// ────────────────────────────────────────────────
// Interfaces
// ────────────────────────────────────────────────

/**
    @brief Client-side interface for the GameName mini-game.
    
    This structure should be registered in the lobby to handle
    initialization, rendering, updating, and network data reception
    for the client side of the game.
*/
extern GameClientInterface_St gameNameClientInterface;

/**
    @brief Server-side interface for the GameName mini-game.
    
    This structure should be registered in the server to handle
    room instance creation, game logic, and action processing
    for the server side of the game.
*/
extern GameServerInterface_St gameNameServerInterface;

#endif // GAME_NAME_API_H
