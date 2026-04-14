/**
    @file main.c
    @author Charles CAILLON
    @date 2026-03-18
    @date 2026-04-14
    @brief Standalone entry point for testing the King-for-Four game.
*/
#include <stddef.h>
#include "raylib.h"

#include "kingForFourAPI.h"
#include "APIs/generalAPI.h"

#include "logger.h"

/**
 * @brief Main function for the standalone King-for-Four test application.
 * @return 0 on successful exit, 1 on initialization error.
 */
int main(void) {
    // Initialisation Window
    InitWindow(1280, 720, "King For Four - Standalone Test");
    SetTargetFPS(60);

    KingForFourGame_St* game = NULL;
    
    // Initialisation du jeu via votre API
    Error_Et err = kingforfour_initGame(&game);
    if (err != OK) {
        TraceLog(LOG_ERROR, "Erreur lors de l'initialisation du jeu");
        CloseWindow();
        return 1;
    }

    // Boucle de jeu (Frame par Frame)
    while (!WindowShouldClose()) {
        kingforfour_gameLoop(game);
        
        // Arrêt si le jeu signale running = false (via cast BaseGame_St)
        if (!((BaseGame_St*)game)->running) break;
    }

    // Nettoyage final
    kingforfour_freeGame(&game);
    CloseWindow();
    
    return 0;
}

#define LOGGER_IMPLEMENTATION
#include "logger.h"