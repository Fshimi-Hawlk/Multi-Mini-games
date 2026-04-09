#ifndef AMBIANCE_H
#define AMBIANCE_H

#include "utils/userTypes.h"

void updateAtmosphericEffects(float dt, Player_St* player, Camera2D cam);
void drawAtmosphericEffects(void);
void drawScreenEffects(Player_St* player);

#endif // AMBIANCE_H