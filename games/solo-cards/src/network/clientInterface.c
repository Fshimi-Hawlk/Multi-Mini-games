/**
    @file clientInterface.c
    @author Maxime CHAUVEAU
    @date 2026-04-14
    @brief Lobby-compatible client interface for Solo Cards.
*/
#include "solitaireAPI.h"

#include "APIs/generalAPI.h"
#include "networkInterface.h"
#include "logger.h"

static SolitaireGame_St* solitaire_game = NULL;

void solitaire_lobbyInit(void) {
    if (solitaire_initGame__full(&solitaire_game, (SoloCardsConfig_St){0}) != OK) {
        log_error("[SOLO-CARDS] Failed to initialize game");
    }
}

void solitaire_lobbyUpdate(float dt) {
    (void)dt;

    if (solitaire_game) {
        solitaire_gameLoop(solitaire_game);
    }
}

void solitaire_lobbyDraw(void) {
}

void solitaire_lobbyDestroy(void) {
    if (solitaire_game) {
        solitaire_freeGame(&solitaire_game);
    }
}

GameClientInterface_St soloCards_clientInterface = {
    .id = MINI_GAME_ID_SOLO_CARDS,
    .name = "Solo Cards",
    .init = solitaire_lobbyInit,
    .onData = NULL,
    .update = solitaire_lobbyUpdate,
    .draw = solitaire_lobbyDraw,
    .destroy = solitaire_lobbyDestroy
};