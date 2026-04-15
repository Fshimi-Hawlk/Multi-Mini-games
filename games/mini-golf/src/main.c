/**
    @file main.c
    @author Maxime CHAUVEAU
    @date 2026-04-07
    @date 2026-04-14
    @brief Entry point for the Golf 3D mini-game (standalone mode).
*/
#include "golf.h"

/**
    @brief Main entry point for the standalone Golf game.

    @return 0 on successful exit.
*/
int main(void) {
    GolfGame game;

    InitWindow(SCREEN_W, SCREEN_H, GAME_TITLE);
    SetTargetFPS(TARGET_FPS);
    SetExitKey(KEY_NULL);

    Game_Init(&game);

    while (!WindowShouldClose()) {
        float dt = GetFrameTime();
        if (dt > 0.05f) dt = 0.05f;
        Game_Update(&game, dt);
        Game_Draw(&game);
    }

    Game_Cleanup(&game);
    CloseWindow();
    return 0;
}
