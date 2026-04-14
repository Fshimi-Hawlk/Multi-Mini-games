/**
    @file ambiance.h
    @author Léandre BAUDET
    @date 2026-04-14
    @date 2026-04-14
    @brief ambiance.h implementation/header file
*/
#ifndef UI_AMBIANCE_H
#define UI_AMBIANCE_H

#include "utils/userTypes.h"

/**
    @brief Description for lobby_updateAtmosphericEffects
    @param[in,out] dt The dt parameter
    @param[in,out] player The player parameter
    @param[in,out] cam The cam parameter
*/
void lobby_updateAtmosphericEffects(float dt, Player_St* player, Camera2D cam);
/**
    @brief Description for lobby_drawAtmosphericEffects
*/
void lobby_drawAtmosphericEffects(void);
/**
    @brief Description for lobby_drawScreenEffects
    @param[in,out] player The player parameter
*/
void lobby_drawScreenEffects(Player_St* player);

#endif // UI_AMBIANCE_H
