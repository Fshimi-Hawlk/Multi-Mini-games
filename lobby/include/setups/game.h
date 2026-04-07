/**
    @file setups/game.h
    @author Grok (assisted)
    @date 2026-03-30
    @brief Central game state and initialization for the lobby.
*/

#ifndef SETUPS_GAME_H
#define SETUPS_GAME_H

#include "APIs/generalAPI.h"
#include "utils/userTypes.h"

/**
    @brief Initializes all game-related data (dynamic terrains, etc.).
*/
Error_Et gameInit(void);

#endif // SETUPS_GAME_H
