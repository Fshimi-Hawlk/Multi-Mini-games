#ifndef AMBIANCE_H
#define AMBIANCE_H

#include "types.h"

void initAtmosphericEffects(void);
void updateAtmosphericEffects(float dt, Player_st* player, Camera2D cam);
void drawAtmosphericEffects(void);
void drawScreenEffects(Player_st* player);

#endif // AMBIANCE_H