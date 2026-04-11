/**
 * @file main.c
 * @brief Solitaire game entry point
 * @author Maxime CHAUVEAU
 * @date February 2026
 */

#include "solitaire.h"
#include <stdio.h>

#include "utils/audio.h"

int main(void) {
    // Initialize window
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Solitaire - Multi Mini-Games");
    SetTargetFPS(TARGET_FPS);

    InitAudioDevice();
    solitaire_initAudio();

    // Initialize game
    SolitaireGameState game = {0};
    solitaire_init(&game);

    // Main game loop
    while (!WindowShouldClose()) {
        float deltaTime = GetFrameTime();

        // Update
        solitaire_update(&game, deltaTime);

        // Draw
        BeginDrawing();
        solitaire_draw(&game);
        EndDrawing();
    }

    // Cleanup
    CloseAudioDevice();
    solitaire_freeAudio();

    solitaire_cleanup(&game);
    CloseWindow();

    return 0;
}
