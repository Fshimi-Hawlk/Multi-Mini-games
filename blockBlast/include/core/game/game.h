#ifndef CORE_GAME_GAME_H
#define CORE_GAME_GAME_H

#include "utils/userTypes.h"

void buildScoreText(void);
f32 calculateScore(const Board_St* const board);
void manageScore(GameState_St* const game, const Prefab_St* const prefab);

#endif // CORE_GAME_GAME_H