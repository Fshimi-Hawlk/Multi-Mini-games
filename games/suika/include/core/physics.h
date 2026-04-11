/**
    @file physics.h
    @brief Physics simulation declarations for the Suika game.
    @author Multi Mini-Games Team
    @date February 2026

    Declares physics functions for fruit collision detection, gravity,
    and movement simulation.
*/

#ifndef SUIKA_CORE_PHYSICS_H
#define SUIKA_CORE_PHYSICS_H

#include "utils/types.h"

/**
    @brief Run physics simulation for one frame.

    Updates velocities, positions, and handles collisions between fruits
    and with container walls.

    @param game Pointer to the game state
    @param deltaTime Time elapsed since last frame in seconds
*/
void suika_updatePhysics(SuikaGame_St* game, float deltaTime);

/**
    @brief Check for and process fruit merges.

    Detects collisions between same-type fruits and merges them
    into the next larger fruit type.

    @param game Pointer to the game state
*/
void suika_checkMerging(SuikaGame_St* game);

/**
    @brief Check for game over condition.

    Detects if fruits have been above the drop line for too long.

    @param game Pointer to the game state
*/
void suika_checkGameOver(SuikaGame_St* game);

#endif
