#ifndef UI_GAME_H
#define UI_GAME_H

#include "utils/userTypes.h"

void drawPlayer(const Player_st* const player);
void drawPlatforms(const Platform_st* const platforms, const int nbPlatforms);

#endif // UI_GAME_H