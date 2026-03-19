/**
 * @file main.c
 * @brief Puissance4 game entry point
 * @author Maxime CHAUVEAU
 * @date March 2026
 * @version 1.0.0
 * 
 * @section description Description
 * 
 * This is the main entry point for the Puissance4 (Connect Four) game.
 * It initializes the game window, creates a game instance, and runs
 * the main game loop until the window is closed.
 * 
 * @section initialization Initialization
 * 
 * The initialization process includes:
 * - Creating a window with specified dimensions (1000x800)
 * - Setting target FPS to 60
 * - Initializing the game with default configuration
 * 
 * @section game_loop Main Game Loop
 * 
 * The main loop continues until either:
 * - The user closes the window (WindowShouldClose())
 * - The game signals it is no longer running (puissance4_isRunning)
 * 
 * @section cleanup Cleanup
 * 
 * On exit, the following cleanup is performed:
 * - Free game resources (puissance4_freeGame)
 * - Close the graphics window (CloseWindow)
 */

#include "puissance4API.h"
#include <stdio.h>

int main(void) {
    // Initialize window
    const int screenWidth = 1000;
    const int screenHeight = 800;
    InitWindow(screenWidth, screenHeight, "Puissance4 - Multi Mini-Games");
    SetTargetFPS(60);

    // Initialize game
    Puissance4Game_St* game = puissance4_initGame(NULL);
    if (!game) {
        printf("Failed to initialize game\n");
        return 1;
    }

    // Main game loop
    while (!WindowShouldClose() && puissance4_isRunning(game)) {
        puissance4_gameLoop(game);
    }

    // Cleanup
    puissance4_freeGame(game);
    CloseWindow();

    return 0;
}
