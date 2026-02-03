/**
 * @file main.c
 * @brief Suika game entry point
 * @author Multi Mini-Games Team
 * @date February 2026
 */

#include "suika.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(void) {
    // Initialize random seed
    srand((unsigned int)time(NULL));
    
    // Initialize window
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Suika Game - Multi Mini-Games");
    SetTargetFPS(TARGET_FPS);

    // Initialize game
    SuikaGame_St game = {0};
    suika_init(&game);

    // Main game loop
    while (!WindowShouldClose()) {
        float deltaTime = GetFrameTime();

        // Update
        suika_update(&game, deltaTime);

        // Draw
        BeginDrawing();
        suika_draw(&game);
        EndDrawing();
    }

    // Cleanup
    suika_cleanup(&game);
    CloseWindow();

    return 0;
}
