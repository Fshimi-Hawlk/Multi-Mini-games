#include "raylib.h"
#include "setups/app.h"

int main(void) {
    initApp();

    while (!WindowShouldClose()) {
        BeginDrawing(); {
            ClearBackground(WHITE);
        } EndDrawing();
    }

    freeApp();

    return 0;
}