#ifndef RUBIKSCUBE_LOBBY_API_H
#define RUBIKSCUBE_LOBBY_API_H
#include "baseTypes.h"
#include "APIs/generalAPI.h"
typedef struct RubiksCubeAPI_St RubiksCubeAPI_St;
typedef struct {
    unsigned int fps;
    int width;
    int height;
} RubiksCubeConfigs_St;
#define rubikscube_initGame(game, ...) \
    rubikscube_initGame__full((game), (RubiksCubeConfigs_St){ .fps = 60, ##__VA_ARGS__ })
Error_Et rubikscube_initGame__full(RubiksCubeAPI_St** game, RubiksCubeConfigs_St configs);
Error_Et rubikscube_gameLoop(RubiksCubeAPI_St* const game);
Error_Et rubikscube_freeGame(RubiksCubeAPI_St** game);
bool     rubikscube_isRunning(const RubiksCubeAPI_St* game);
#endif
