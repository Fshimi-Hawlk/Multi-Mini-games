#ifndef GRASS_H
#define GRASS_H

#include "types.h"

void initGrass(void);
void updateGrass(Player_st* player, float dt, float time, Camera2D camera);
void drawGrass(Player_st* player, Camera2D camera);

#endif // GRASS_H