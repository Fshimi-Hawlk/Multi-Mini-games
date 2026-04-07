#ifndef RUBIKS_CUBE_API_H
#define RUBIKS_CUBE_API_H

#include "core/game.h"
#include "APIs/gameConfig.h"
#include "APIs/generalAPI.h"

typedef struct RubiksCubeAPI_St RubiksCubeAPI_St;

typedef struct {
    int width;
    int height;
    int fps;
} RubiksCubeConfigs_St;

Error_Et rubikscube_initGame__full(RubiksCubeAPI_St** game, RubiksCubeConfigs_St cfg);
Error_Et rubikscube_gameLoop(RubiksCubeAPI_St* const g);
Error_Et rubikscube_freeGame(RubiksCubeAPI_St** game);
bool rubikscube_isRunning(const RubiksCubeAPI_St* g);

#endif