#include "core/game.h"
#include "ui/render.h"
#include <stdio.h>

int main(void) {
    const int screenWidth = 1000;
    const int screenHeight = 800;
    InitWindow(screenWidth, screenHeight, "Puissance4 - Multi Mini-Games");
    SetTargetFPS(60);

    Puissance4Game_St* game = puissance4Core_initGame(NULL);
    if (!game) {
        printf("Failed to initialize game\n");
        return 1;
    }

    while (!WindowShouldClose() && puissance4Core_isRunning(game)) {
        puissance4Core_gameLoop(game);
    }

    puissance4Core_freeGame(game);
    CloseWindow();

    return 0;
}