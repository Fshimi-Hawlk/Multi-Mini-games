/**
    @file main.c
    @author Léandre BAUDET
    @date 2026-03-10
    @date 2026-04-14
    @brief Entry point for the Snake mini-game (standalone mode).
*/
#include "APIs/generalAPI.h"
#include "raylib.h"
#include "utils/common.h"

#include "snakeAPI.h"

int main(void) {
    srand(time(NULL));
    SetTraceLogLevel(LOG_WARNING);
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Snake");
    SetTargetFPS(60);

    SnakeGame_St* game = NULL;
    snake_initGame(&game);

    while (!WindowShouldClose() && ((BaseGame_St*) game)->running) {
        snake_gameLoop(game);
    }
    
    snake_freeGame(&game);

    CloseWindow();
    return 0;
}

#define PARAMS_MENU_IMPLEMENTATION
#include "paramsMenu.h"