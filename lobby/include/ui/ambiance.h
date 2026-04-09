#ifndef UI_AMBIANCE_H
#define UI_AMBIANCE_H

#include "utils/userTypes.h"

void lobby_updateAtmosphericEffects(float dt, Player_St* player, Camera2D cam);
void lobby_drawAtmosphericEffects(void);
void lobby_drawScreenEffects(Player_St* player);

#endif // UI_AMBIANCE_H
