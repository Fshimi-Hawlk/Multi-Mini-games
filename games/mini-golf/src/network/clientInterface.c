/**
    @file clientInterface.c
    @author Maxime CHAUVEAU
    @date 2026-04-14
    @brief Lobby-compatible client interface for Mini-Golf.
*/
#include "golf.h"
#include "golfAPI.h"

#include "APIs/generalAPI.h"
#include "networkInterface.h"

static GolfGame_St* golf_game = NULL;

void golf_lobbyInit(void) {
    if (golf_initGame__full(&golf_game, (GolfConfigs_St){0}) != OK) {
    }
}

void golf_lobbyUpdate(float dt) {
    (void)dt;

    if (golf_game) {
        golf_gameLoop(golf_game);
    }
}

void golf_lobbyDraw(void) {
}

void golf_lobbyDestroy(void) {
    if (golf_game) {
        golf_freeGame(&golf_game);
    }
}

GameClientInterface_St miniGolf_clientInterface = {
    .id = MINI_GAME_ID_MINI_GOLF,
    .name = "Mini-Golf",
    .init = golf_lobbyInit,
    .onData = NULL,
    .update = golf_lobbyUpdate,
    .draw = golf_lobbyDraw,
    .destroy = golf_lobbyDestroy
};