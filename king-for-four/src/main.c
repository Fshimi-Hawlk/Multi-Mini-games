#define LOGGER_IMPLEMENTATION
#include <stddef.h>
#include "raylib.h"
#include "kingforfourAPI.h"
#include "logger.h"
#include "APIs/generalAPI.h"

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