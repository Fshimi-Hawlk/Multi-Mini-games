#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "config.h"

#include "raylib.h"

int main() {
    SetTraceLogLevel(LOG_WARNING);

    InitWindow(screenWidth, screenHeight, "Basic Raylib Window");

    while (!WindowShouldClose()) {
        BeginDrawing();

        ClearBackground(RAYWHITE);
        
        EndDrawing();
    }
    
    CloseWindow();
    return 0;
}

