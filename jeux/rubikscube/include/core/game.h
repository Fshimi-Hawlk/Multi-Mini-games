#ifndef CORE_GAME_H
#define CORE_GAME_H

#include <stdbool.h>
#include "APIs/gameConfig.h"

typedef struct RubiksCubeGame_St RubiksCubeGame_St;

RubiksCubeGame_St* rubiksCube_initGame(const GameConfig_St* config);
bool rubiksCube_isRunning(const RubiksCubeGame_St* game);
void rubiksCube_gameLoop(RubiksCubeGame_St* const game);
void rubiksCube_freeGame(RubiksCubeGame_St* game);

#endif