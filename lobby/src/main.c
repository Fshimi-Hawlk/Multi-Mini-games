#include "core/game.h"

#include "ui/app.h"
#include "ui/game.h"

#include "utils/globals.h"

Camera2D cam = {0};

int main(void) {
    SetTraceLogLevel(LOG_WARNING);

    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Basic Raylib Window");

    logoSkinButton = LoadTexture("assets/logoSkin.png");

    playerTextures[0] = LoadTexture("assets/earth.png"); playerTextureCount++;
    playerTextures[1] = LoadTexture("assets/trollFace.png"); playerTextureCount++;
    
    Player_st player = {
        .position = {0, 300},
        .radius = 20,

        .texture = NULL,
        .angle = 0,

        .velocity = {0, 0},

        .onGround = false,
        .nbJumps = 0,

        .coyoteTime = 0.1f,
        .coyoteTimer = 0.1f,

        .jumpBuffer = 0.2f
    };

    cam.target = player.position;
    cam.offset = (Vector2) {WINDOW_WIDTH / 2.0f, WINDOW_HEIGHT / 2.0f};
    cam.zoom = 1.0f;

    while (!WindowShouldClose()) {
        float dt = GetFrameTime();

        updatePlayer(&player, platforms, platformCount, dt);

        cam.target = player.position;

        toggleSkinMenu();

        if (isTextureMenuOpen) {
            choosePlayerTexture(&player);
        }

        BeginDrawing(); {
            ClearBackground(RAYWHITE);
            BeginMode2D(cam); {
                drawPlayer(&player);
                drawPlatforms(platforms, platformCount);
            } EndMode2D();

            DrawText("Multi-Mini-Games", WINDOW_WIDTH / 2 - MeasureText("Multi-Mini-Games", 20) / 2, 20, 20, PURPLE);
            drawSkinButton();
            
            if (isTextureMenuOpen) {
                drawMenuTextures();
            }
        } EndDrawing();
    }

    for (int i = 0; i < playerTextureCount; i++)
        UnloadTexture(playerTextures[i]);
    
    UnloadTexture(logoSkinButton);

    CloseWindow();
    return 0;
}

#define LOGGER_IMPLEMENTATION
#include "logger.h"