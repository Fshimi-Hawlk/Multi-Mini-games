/**
    @file ui/app.c
    @author Maxime CHAUVEAU
    @date 2026-02-25
    @brief Rendering logic for the player skin/character selection menu and HUD in the lobby.

    This file contains the drawing functions that visualize:
        - the skin selection overlay / menu (grid of available player textures)
        - the skin-button icon that opens/closes the selection menu
        - the game HUD (title, instructions)

    Functions in this file:
        - operate in screen-space coordinates
        - expect pre-loaded textures in game->playerVisuals.textures[]
        - should be called during the UI render pass (after world rendering)
        - perform no state changes - pure draw calls

    Typical usage in the lobby render loop:
        BeginDrawing();
            // world rendering (BeginMode2D / EndMode2D)
            if (game->playerVisuals.isTextureMenuOpen)
                drawMenuTextures(game);
            drawSkinButton();
            drawGameHUD(game);
        EndDrawing();

    @see `utils/globals.h`     for `logoSkinButton`, `skinButtonRect`
    @see `utils/types.h`       for `LobbyGame_St`, `PlayerVisuals_St`, `__playerTextureCount`
    @see `utils/utils.h`       for `getTextureRec()`
 */

#include "ui/app.h"

#include "utils/globals.h"
#include "utils/utils.h"
#include "utils/configs.h"

void drawMenuTextures(const LobbyGame_St* const game) {
    Rectangle destRect = game->playerVisuals.defaultTextureRect;

    // Background panel
    DrawRectangle(10, 30, 280, 100, (Color){30, 30, 30, 200});
    DrawRectangleLinesEx((Rectangle){10, 30, 280, 100}, 2, DARKGRAY);
    
    // Default texture
    f32 radius = destRect.width / 2.0f;
    DrawCircleV((Vector2) {destRect.x + radius, destRect.y + radius}, radius, BLUE);

    
    DrawText("choose your skin :", 20, 40, 16, LIGHTGRAY);
    for (u32 i = 1; i < __playerTextureCount; i++) {
        Color textureTint = game->player.unlockedTextures[i] ? WHITE : GRAY;
        destRect.x = 20 + i * 60;
        DrawTexturePro(
            game->playerVisuals.textures[i],
            getTextureRec(game->playerVisuals.textures[i]),
            destRect,
            Vector2Zero(),
            0,
            textureTint
        );
        
        // Draw lock indicator if not unlocked
        if (!game->player.unlockedTextures[i]) {
            DrawText("X", (int)(destRect.x + destRect.width/2 - 5), (int)(destRect.y + destRect.height/2 - 8), 16, RED);
        }
    }
}

void drawSkinButton(void) {
    DrawTexturePro(
        logoSkinButton,
        getTextureRec(logoSkinButton),
        skinButtonRect,
        Vector2Zero(),
        0,
        WHITE
    );
}

void drawGameHUD(const LobbyGame_St* const game) {
    (void)game; // Unused for now
    
    // Draw title
    f32 titleXPos = (GetScreenWidth() - MeasureText("Multi-Mini-Games", 24)) / 2.0f;
    DrawText("Multi-Mini-Games", (int)titleXPos, 20, 24, PURPLE);
    
    // Draw subtitle
    const char* subtitle = "Lobby - Explore and play mini-games!";
    f32 subtitleXPos = (GetScreenWidth() - MeasureText(subtitle, 16)) / 2.0f;
    DrawText(subtitle, (int)subtitleXPos, 50, 16, LIGHTGRAY);
    
    // Draw controls hint at bottom
    const char* controls = "WASD/Arrows: Move | Space/W/Up: Jump | E: Enter game zone | P: Skins";
    f32 controlsXPos = (GetScreenWidth() - MeasureText(controls, 14)) / 2.0f;
    DrawText(controls, (int)controlsXPos, GetScreenHeight() - 30, 14, GRAY);
}
