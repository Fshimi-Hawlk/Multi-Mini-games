/**
    @file physics.h
    @author Maxime CHAUVEAU
    @date 2026-02-01
    @date 2026-04-14
    @brief Physics simulation declarations for the Suika game.
*/
#ifndef SUIKA_CORE_PHYSICS_H
#define SUIKA_CORE_PHYSICS_H

#include "utils/types.h"

/**
    @brief Run physics simulation for one frame.

    Updates velocities, positions, and handles collisions between fruits
    and with container walls.

    @param[in,out] game      Pointer to the game state
    @param[in]     deltaTime Time elapsed since last frame in seconds
    @return                  void
*/
void suika_updatePhysics(SuikaGame_St* game, float deltaTime);

/**
    @brief Check for and process fruit merges.

    Detects collisions between same-type fruits and merges them
    into the next larger fruit type.

    @param[in,out] game Pointer to the game state
    @return            void
*/
void suika_checkMerging(SuikaGame_St* game);

/**
    @brief Check for game over condition.

    Detects if fruits have been above the drop line for too long.

    @param[in,out] game Pointer to the game state
    @return            void
*/
void suika_checkGameOver(SuikaGame_St* game);

#endif
