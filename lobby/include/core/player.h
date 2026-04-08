#ifndef PLAYER_H
#define PLAYER_H

#include "types.h"

void updatePlayer(Player_St* player, Platform_St* platforms, int nbPlatforms, float dt);

/**
    @brief Draws the player with a soft, position-dependent atmospheric glow.
    The glow becomes stronger when the player is higher in the air (closer to the stars).
    Completely replaces the previous rim-light that looked bad.
*/
void drawPlayer(Player_St* player);

#endif // PLAYER_H