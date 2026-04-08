#include "core/game.h"

#include "raylib.h"
#include "setups/app.h"
#include "setups/game.h"
#include "setups/audio.h"

#include "ui/grass.h"
#include "ui/background.h"
#include "ui/ambiance.h"
#include "ui/game.h"
#include "ui/app.h"

#include "utils/globals.h"

int main(void) {
    lobby_initApp();
    lobby_initGame();

    while (!WindowShouldClose()) {
        float dt = GetFrameTime();
        gameTime += dt;

        if (gameTime > 1.45f) {
            updatePlayer(&game.player, platforms, platformCount, dt);
        }

        game.cam.target = game.player.position;

        toggleSkinMenu(&game);

        if (game.playerVisuals.isTextureMenuOpen) {
            choosePlayerTexture(&game.player, &game);
        }

        updateGrass(&game.player, GetFrameTime(), gameTime, game.cam);
        updateAtmosphericEffects(dt, &game.player, game.cam);

        BeginDrawing(); {
            ClearBackground(RAYWHITE);

            BeginMode2D(game.cam); {
                drawStarryBackground(game.player.position, game.cam);

                drawTree();
                
                drawPlatforms(platforms, platformCount);
                drawPlayer(&game, &game.player);
                
                drawWorldBoundaries(&game.player);
                
                drawGrass(&game.player, game.cam);
                drawAtmosphericEffects();
            } EndMode2D();

            drawScreenEffects(&game.player);

            DrawText("Multi-Mini-Games", WINDOW_WIDTH / 2.0f - MeasureText("Multi-Mini-Games", 20) / 2.0f, 20, 20, PURPLE);
            drawSkinButton();
            if (game.playerVisuals.isTextureMenuOpen) {
                drawMenuTextures(&game);
            }

        } EndDrawing();
    }

    for (int i = 0; i < __playerTextureCount; i++)
        UnloadTexture(game.playerVisuals.textures[i]);

    UnloadTexture(logoSkinButton);

    lobby_freeAudio();
    lobby_freeTextures();

    CloseWindow();
    return 0;
}