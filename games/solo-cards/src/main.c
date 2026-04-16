/**
    @file main.c
    @author Maxime CHAUVEAU
    @date 2026-02-01
    @date 2026-04-14
    @brief Solitaire game entry point
*/
#include "solitaire.h"
#include <stdio.h>

#include "utils/audio.h"

/**
    @brief Game entry point.
    @return 0 on successful exit.
*/
int main(void) {
    // Initialize window
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Solitaire - Multi Mini-Games");
    SetTargetFPS(TARGET_FPS);

    InitAudioDevice();
    solitaire_initAudio();

    // Initialize game
    SolitaireGameState_St game = {0};
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
