/**
    @file setups/game.h
    @author Grok (assisted)
    @date 2026-03-27
    @brief Central game state and initialization for the lobby.

    Manages dynamic terrain array and provides init/free functions.
    Replaces scattered global initialization from app.c and clientInterface.c.
*/

#ifndef SETUPS_GAME_H
#define SETUPS_GAME_H

#include "APIs/generalAPI.h"

/**
    @brief Initializes all game-related data (dynamic terrains, etc.).
    Should be called after window creation but before editor or gameplay starts.
*/
Error_Et gameInit(void);

#endif // SETUPS_GAME_H