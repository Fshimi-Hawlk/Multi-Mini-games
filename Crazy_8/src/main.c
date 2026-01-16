#include "raylib.h"

int main(void)
{
    const int screenWidth = 1280;
    const int screenHeight = 720;


    InitWindow(screenWidth, screenHeight, "Raylib - Dynamic FPS & Delta Time");

    // Variables de mouvement
    float ballPositionX = 100.0f;
    float speed = 300.0f; // 300 pixels par seconde


    while (!WindowShouldClose())
    {
        // 1. Calcul du Delta Time
        float deltaTime = GetFrameTime(); 

        // 2. Mise à jour (Update)
        ballPositionX += speed * deltaTime;

        // Rebondir sur les bords
        if (ballPositionX > screenWidth || ballPositionX < 0) speed *= -1;

        // 3. Dessin (Draw)
        BeginDrawing();
            ClearBackground(RAYWHITE);

            // Affiche les FPS actuels en haut à gauche
            DrawFPS(10, 10);

            DrawText("Vitesse fixe malgré des FPS variables !", 190, 150, 20, DARKGRAY);
            DrawCircle((int)ballPositionX, screenHeight / 2, 40, MAROON);

        EndDrawing();
    }

    CloseWindow();
    return 0;
}