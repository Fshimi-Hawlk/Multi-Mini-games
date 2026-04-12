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

#include "sharedUtils/geometry.h"
#include "sharedUtils/debug.h"

void lobby_drawMenuTextures(const LobbyGame_St* const game) {
    Rectangle destRect = game->playerVisuals.defaultTextureRect;

    // Default texture
    f32 radius = destRect.width / 2.0f;
    DrawCircleV((Vector2) {destRect.x + radius, destRect.y + radius}, radius, BLUE);

    
    DrawText("choose your skin :", 20, 40, 20, DARKGRAY);
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
    }
}

void lobby_drawSkinButton(void) {
    DrawTexturePro(
        logoSkinButton,
        getTextureRec(logoSkinButton),
        skinButtonRect,
        Vector2Zero(),
        0,
        WHITE
    );
}

// ────────────────────────────────────────────────
// Physics Debug Panel
// ────────────────────────────────────────────────

void drawPhysicsDebugPanel(const Player_St* const player, const PhysicsConstants_St* const pc) {
    if (!showPhysicsDebugPanel) return;

    UNUSED(pc);

    const f32 panelW = 350.0f;
    const f32 panelH = 500.0f;
    const f32 panelX = (f32)GetScreenWidth() - panelW - 20.0f;
    const f32 panelY = 80.0f;

    DrawRectangleRounded((Rectangle){panelX, panelY, panelW, panelH}, 0.1f, 10, Fade(BLACK, 0.85f));
    DrawRectangleLinesEx((Rectangle){panelX, panelY, panelW, panelH}, 2, SKYBLUE);
    DrawText("PHYSICS SETTINGS (Skin)", (int)panelX + 20, (int)panelY + 15, 20, SKYBLUE);

    const char *skinNames[__playerTextureCount] = {
        [PLAYER_TEXTURE_DEFAULT]        = "Default",
        [PLAYER_TEXTURE_BINGO]          = "Bingo",
        [PLAYER_TEXTURE_BOWLING]        = "Bowling",
        [PLAYER_TEXTURE_CHESS]          = "Chess",
        [PLAYER_TEXTURE_DROP_FOUR]      = "Drop Four",
        [PLAYER_TEXTURE_TWIST_CUBE]     = "Twist Cube",
        [PLAYER_TEXTURE_EDITOR]         = "Editor",
        [PLAYER_TEXTURE_KING_FOR_FOUR]  = "King for Four",
        [PLAYER_TEXTURE_LOBBY]          = "Earth",
        [PLAYER_TEXTURE_MINI_GOLF]      = "Mini Golf",
        [PLAYER_TEXTURE_DISC_REVERSAL]  = "Disc Reversal",
        [PLAYER_TEXTURE_POLY_BLAST]     = "Poly Blast",
        [PLAYER_TEXTURE_SNAKE]          = "Snake",
        [PLAYER_TEXTURE_SOLO_CARDS]     = "Solo Cards",
        [PLAYER_TEXTURE_SUIKA]          = "Suika",
        [PLAYER_TEXTURE_TETROMINO_FALL] = "Tetromino Fall",
    };

    const char *skinName = skinNames[player->textureId];

    DrawText(TextFormat("Skin: %s", skinName), (int)panelX + 20, (int)panelY + 45, 18, GOLD);
    
    // ... List of constants would go here ...
    DrawText("Press F2 to close", (int)panelX + 20, (int)panelY + panelH - 30, 16, GRAY);
}