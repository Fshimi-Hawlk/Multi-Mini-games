/**
    @file grass.h
    @author Léandre BAUDET
    @date 2026-04-14
    @date 2026-04-14
    @brief grass.h implementation/header file
*/
#ifndef UI_GRASS_H
#define UI_GRASS_H

#include "utils/userTypes.h"

/**
    @brief Description for lobby_updateGrass
    @param[in,out] player The player parameter
    @param[in,out] dt The dt parameter
    @param[in,out] time The time parameter
    @param[in,out] camera The camera parameter
*/
void lobby_updateGrass(const Player_St* const player, const float dt, const float time, const  Camera2D camera);

/**
    @brief Description for lobby_drawGrass
    @param[in,out] camera The camera parameter
*/
void lobby_drawGrass(const Camera2D camera);

#endif // UI_GRASS_H