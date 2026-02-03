/**
 * @file main.c
 * @brief Bowling game entry point
 * @author Multi Mini-Games Team
 * @date February 2026
 */

#include "bowling.h"
#include <stdio.h>

int main(void) {
    // Initialize window
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Bowling - Multi Mini-Games");
    SetTargetFPS(TARGET_FPS);

    // Initialize game
    BowlingGame_St game = {0};
    bowling_init(&game);

    // Main game loop
    while (!WindowShouldClose()) {
        float deltaTime = GetFrameTime();

        // Update
        bowling_update(&game, deltaTime);

        // Draw
        BeginDrawing();
        ClearBackground(RAYWHITE);
        
        bowling_draw(&game);
        
        EndDrawing();
    }

    // Cleanup
    bowling_cleanup(&game);
    CloseWindow();

    return 0;
}
