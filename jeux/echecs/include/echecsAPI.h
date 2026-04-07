#ifndef ECHECS_LOBBY_API_H
#define ECHECS_LOBBY_API_H
#include "baseTypes.h"
#include "APIs/generalAPI.h"
typedef struct EchecsAPI_St EchecsAPI_St;
typedef struct { char _; } EchecsConfigs_St;
#define echecs_initGame(game, ...) \
    echecs_initGame__full((game), (EchecsConfigs_St){ ._ = 0, ##__VA_ARGS__ })
Error_Et echecs_initGame__full(EchecsAPI_St** game, EchecsConfigs_St configs);
Error_Et echecs_gameLoop(EchecsAPI_St* const game);
Error_Et echecs_freeGame(EchecsAPI_St** game);
bool     echecs_isRunning(const EchecsAPI_St* game);
#endif
