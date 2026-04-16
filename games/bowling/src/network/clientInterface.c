/**
    @file clientInterface.c
    @author Maxime CHAUVEAU
    @date 2026-04-14
    @brief Lobby-compatible client interface for Bowling.
*/
#include "core/game.h"

#include "APIs/generalAPI.h"
#include "networkInterface.h"
#include <stdio.h>

static BowlingGame_St* bowling_game = NULL;

void bowling_lobbyInit(void) {
    if (bowling_initGame__full(&bowling_game, (BowlingConfigs_St){0}) != OK) {
        fprintf(stderr, "[BOWLING] Failed to initialize game\n");
    }
}

void bowling_lobbyUpdate(float dt) {
    if (bowling_game) {
        bowling_update(bowling_game, dt);
    }
}

void bowling_lobbyDraw(void) {
    if (bowling_game) {
        bowling_draw(bowling_game);
    }
}

void bowling_lobbyDestroy(void) {
    if (bowling_game) {
        bowling_freeGame(&bowling_game);
    }
}

GameClientInterface_St bowling_clientInterface = {
    .id = MINI_GAME_ID_BOWLING,
    .name = "Bowling",
    .init = bowling_lobbyInit,
    .onData = NULL,
    .update = bowling_lobbyUpdate,
    .draw = bowling_lobbyDraw,
    .destroy = bowling_lobbyDestroy
};