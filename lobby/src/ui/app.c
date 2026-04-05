/**
    @file ui/app.c
    @author Fshimi-Hawlk
    @date 2026-02-08
    @date 2026-02-23
    @brief Rendering logic for the player skin/character selection menu in the lobby.

    Contributors:
        - LeandreB8:
            - Provided the initial drawing logic
        - Fshimi-Hawlk:
            - Implementation of skin preview grid and default texture fallback visualization
            - Skin selection button rendering

    This file contains the drawing functions that visualize:
        - the skin selection overlay / menu (grid of available player textures)
        - the skin-button icon that opens/closes the selection menu

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
        EndDrawing();

    @see `utils/globals.h`     for `logoSkinButton`, `skinButtonRect`
    @see `utils/userTypes.h`   for `LobbyGame_St`, `PlayerVisuals_St`, `__playerTextureCount`
    @see `utils/utils.h`       for `getTextureRec()`
*/

#include "ui/app.h"

#include "utils/globals.h"

#include "utils/geometry.h"

void drawMenuTextures(const LobbyGame_St* const game) {
    Rectangle destRect = game->playerVisuals.defaultTextureRect;

    // Default texture
    f32 radius = destRect.width / 2.0f;
    
    DrawText("choose your skin :", 20, 40, 20, DARKGRAY);
    for (u32 i = 0; i < __playerTextureCount; i++) {
        destRect.x = 20 + i * 60;

        Texture2D texture = game->playerVisuals.textures[i];
        Color textureTint = game->player.unlockedTextures[i] ? WHITE : GRAY;

        if (!IsTextureValid(texture)) {
            DrawCircleV((Vector2) {destRect.x + radius, destRect.y + radius}, radius, ColorTint(BLUE, textureTint));
            continue;
        }

        DrawTexturePro(
            texture,
            getTextureRec(texture), // source
            destRect, // destination
            Vector2Zero(),
            0,
            textureTint
        );
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
