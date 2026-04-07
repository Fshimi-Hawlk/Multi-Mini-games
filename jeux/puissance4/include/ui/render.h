#ifndef PUISSANCE4_UI_H
#define PUISSANCE4_UI_H

#include <stdbool.h>
#include "core/game.h"

void puissance4UI_draw(const Puissance4Game_St* game);
void puissance4UI_drawBoard(const Puissance4Game_St* game);
void puissance4UI_drawUI(const Puissance4Game_St* game);

#endif