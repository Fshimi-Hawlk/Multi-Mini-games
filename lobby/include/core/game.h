#ifndef CORE_GAME_H
#define CORE_GAME_H

#include "utils/userTypes.h"

void updatePlayer(Player_st* const player, const Platform_st* const platforms, const int nbPlatforms, const float dt);

void resolveCircleRectCollision(Player_st* const player, const Rectangle rect);

void choosePlayerTexture(Player_st* const player);
void toggleSkinMenu(void);

#endif