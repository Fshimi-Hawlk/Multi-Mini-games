/**
 * @file main.c
 * @brief Rubik's Cube game entry point
 * @author Maxime CHAUVEAU
 * @date March 2026
 * 
 * @section overview Overview
 * This is the main entry point for the Rubik's Cube mini-game.
 * It initializes the raylib window, creates the game instance,
 * and runs the main game loop until the player quits.
 * 
 * @section initialization Initialization Sequence
 * 1. Initialize window (1000x800 pixels, 60 FPS target)
 * 2. Create game instance with default configuration
 * 3. Enter main game loop
 * 4. Cleanup resources and close window
 * 
 * @section error_handling Error Handling
 * If game initialization fails, an error message is printed
 * to standard output and the program exits with code 1.
 */

#include "rubiksCubeAPI.h"
#include <stdio.h>

int main(void) {
    // Initialize window
    const int screenWidth = 1000;
    const int screenHeight = 800;
    InitWindow(screenWidth, screenHeight, "Rubik's Cube - Multi Mini-Games");
    SetTargetFPS(60);

    // Initialize game
    RubiksCubeGame_St* game = rubiksCube_initGame(NULL);
    if (!game) {
        printf("Failed to initialize game\n");
        return 1;
    }

    // Main game loop
    while (!WindowShouldClose() && rubiksCube_isRunning(game)) {
        rubiksCube_gameLoop(game);
    }

    // Cleanup
    rubiksCube_freeGame(game);
    CloseWindow();

    return 0;
}
