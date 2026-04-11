/**
    @file golf_standalone.c
    @brief Standalone entry point for Golf 3D (direct execution, not via lobby).
           Only compiled for the standalone binary — not included in libgolf.a.
*/

#include "golf.h"

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
