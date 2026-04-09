#ifndef UI_GRASS_H
#define UI_GRASS_H

#include "utils/userTypes.h"

void updateGrass(const Player_St* const player, const float dt, const float time, const  Camera2D camera);
void drawGrass(const Player_St* const player, const Camera2D camera);

#endif // UI_GRASS_H