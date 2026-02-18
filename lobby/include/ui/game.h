#ifndef UI_GAME_H
#define UI_GAME_H

#include "utils/userTypes.h"

void drawPlayer(const LobbyGame_St* const game);
void drawPlatforms(const Platform_st* const platforms, const int nbPlatforms);

#endif // UI_GAME_H