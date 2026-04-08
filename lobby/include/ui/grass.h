#ifndef UI_GRASS_H
#define UI_GRASS_H

#include "utils/userTypes.h"

void initGrass(void);
void updateGrass(Player_st* player, float dt, float time, Camera2D camera);
void drawGrass(Player_st* player, Camera2D camera);

#endif // UI_GRASS_H
