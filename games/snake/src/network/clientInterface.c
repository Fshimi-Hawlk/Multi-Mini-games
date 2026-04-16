/**
    @file clientInterface.c
    @author Maxime CHAUVEAU
    @date 2026-04-14
    @brief Lobby-compatible client interface for Snake.
*/
#include "snakeAPI.h"

#include "APIs/generalAPI.h"
#include "networkInterface.h"

static SnakeGame_St* snake_game = NULL;

void snake_lobbyInit(void) {
    if (snake_initGame__full(&snake_game, (SnakeConfigs_St){0}) != OK) {
    }
}

void snake_lobbyUpdate(float dt) {
    if (snake_game && snake_isRunning(snake_game)) {
        snake_update(snake_game, dt);
    } else if (snake_game && !snake_isRunning(snake_game)) {
        extern void switchMinigame(u8 gameId);
        switchMinigame(MINI_GAME_ID_LOBBY);
    }
}

void snake_lobbyDraw(void) {
    if (snake_game) {
        snake_draw(snake_game);
    }
}

void snake_lobbyDestroy(void) {
    if (snake_game) {
        snake_freeGame(&snake_game);
    }
}

GameClientInterface_St snake_clientInterface = {
    .id = MINI_GAME_ID_SNAKE,
    .name = "Snake",
    .init = snake_lobbyInit,
    .onData = NULL,
    .update = snake_lobbyUpdate,
    .draw = snake_lobbyDraw,
    .destroy = snake_lobbyDestroy
};