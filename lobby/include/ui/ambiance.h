#ifndef UI_AMBIANCE_H
#define UI_AMBIANCE_H

#include "utils/userTypes.h"

void initAtmosphericEffects(void);
void updateAtmosphericEffects(float dt, Player_st* player, Camera2D cam);
void drawAtmosphericEffects(void);
void drawScreenEffects(const Player_st* player);

#endif // UI_AMBIANCE_H
