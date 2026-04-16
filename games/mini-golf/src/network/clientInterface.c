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
#include <stdio.h>

static GolfGame_St* golf_game = NULL;

void golf_lobbyInit(void) {
    if (golf_initGame__full(&golf_game, (GolfConfigs_St){0}) != OK) {
        fprintf(stderr, "[MINI-GOLF] Failed to initialize game\n");
    }
}

void golf_lobbyUpdate(float dt) {
    if (golf_game) {
        golf_gameUpdate(golf_game, dt);
    }
}

void golf_lobbyDraw(void) {
    if (golf_game) {
        golf_gameDraw(golf_game);
    }
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