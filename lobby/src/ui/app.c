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

#include "core/game.h"
#include "ui/app.h"

#include "utils/globals.h"
#include "utils/utils.h"

#include "systemSettings.h"

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

static const char* textureNames[__playerTextureCount] = {
    "Default",
    "Earth",
    "Troll Face",
    "Battleship",
    "Bingo",
    "Connect 4",
    "KFF",
    "Minigolf",
    "Morpion",
    "Othello"
};

f32 panelScrollY = 0.0f;

void drawPhysicsDebugPanel(LobbyGame_St* const game) {
    if (!showPhysicsDebugPanel) return;

    const f32 panelW = 440.f;
    const f32 panelH = 520.0f;
    const f32 panelX = systemSettings.video.width - panelW - 20.0f;
    const f32 panelY = 20.0f;

    // Background
    DrawRectangle(panelX - 10, panelY - 10, panelW + 20, panelH + 20, Fade(BLACK, 0.88f));
    DrawRectangle(panelX, panelY, panelW, panelH, Fade(DARKGRAY, 0.96f));
    DrawRectangleLinesEx((Rectangle){panelX, panelY, panelW, panelH}, 3.0f, YELLOW);

    DrawTextEx(lobby_fonts[FONT24], "PHYSICS DEBUG (per-skin)", 
               (Vector2){panelX + 20, panelY + 15}, 24, 0, YELLOW);

    // ── Scrollable content area ─────────────────────────────────────────────
    const f32 contentY = panelY + 55.0f;
    const f32 visibleH = panelH - 110.0f;

    BeginScissorMode((int)panelX, (int)contentY, (int)panelW, (int)visibleH);

    f32 y = contentY - panelScrollY;
    const f32 lineH = 24.0f;

    const PlayerTextureId_Et currentSkin = game->player.textureId;
    const PhysicsConstants_St* pc = &game->physics[currentSkin];

    // ── General info ─────────────────────────────────────────────────────
    DrawTextEx(lobby_fonts[FONT18], TextFormat("Skin: %s (%d)", 
               textureNames[currentSkin], currentSkin), 
               (Vector2){panelX + 20, y}, 18, 0, WHITE); 
    y += lineH;

    DrawTextEx(lobby_fonts[FONT18], TextFormat("Pos: %.1f, %.1f", 
               game->player.position.x, game->player.position.y), 
               (Vector2){panelX + 20, y}, 18, 0, LIGHTGRAY); 
    y += lineH;

    DrawTextEx(lobby_fonts[FONT18], TextFormat("Vel: %.1f, %.1f", 
               game->player.velocity.x, game->player.velocity.y), 
               (Vector2){panelX + 20, y}, 18, 0, LIGHTGRAY); 
    y += lineH;

    y += 12.0f;

    // ── Status flags ─────────────────────────────────────────────────────
    Color groundColor = game->player.onGround ? LIME : LIGHTGRAY;
    DrawTextEx(lobby_fonts[FONT18], "On ground", 
               (Vector2){panelX + 20, y}, 18, 0, groundColor); 
    y += lineH;

    Color iceColor = game->player.onIce ? SKYBLUE : LIGHTGRAY;
    DrawTextEx(lobby_fonts[FONT18], "On ice", 
               (Vector2){panelX + 20, y}, 18, 0, iceColor); 
    y += lineH;

    // Water state
    f32 submersion = 0.0f;
    if (game->player.isInWater) {
        for (u32 i = 0; i < terrains.count; ++i) {
            if (terrains.items[i].type == TERRAIN_WATER) {
                f32 s = getWaterSubmersion(&game->player, terrains.items[i].rect);
                if (s > submersion) submersion = s;
            }
        }
    }

    const char* waterState = "Outside water";
    Color waterColor = LIGHTGRAY;
    if (submersion > 0.00095f) {
        waterState = submersion < 0.1f ? "On water surface" : "In water";
        waterColor = submersion < 0.1f ? SKYBLUE : BLUE;
    }

    DrawTextEx(lobby_fonts[FONT18], waterState, 
               (Vector2){panelX + 20, y}, 18, 0, waterColor); 
    y += lineH + 10.0f;

    // ── Current skin physics values ─────────────────────────────────────
    DrawTextEx(lobby_fonts[FONT20], "CURRENT SKIN PHYSICS", 
               (Vector2){panelX + 20, y}, 20, 0, ORANGE); 
    y += lineH + 8.0f;

    // Land / general
    DrawTextEx(lobby_fonts[FONT18], TextFormat("Gravity      : %.1f", pc->gravity), 
               (Vector2){panelX + 20, y}, 18, 0, WHITE); y += lineH;
    DrawTextEx(lobby_fonts[FONT18], TextFormat("Move Speed   : %.1f", pc->moveSpeed), 
               (Vector2){panelX + 20, y}, 18, 0, WHITE); y += lineH;
    DrawTextEx(lobby_fonts[FONT18], TextFormat("Jump Force   : %.1f", pc->jumpForce), 
               (Vector2){panelX + 20, y}, 18, 0, WHITE); y += lineH;
    DrawTextEx(lobby_fonts[FONT18], TextFormat("Friction     : %.1f (Ice: %.1f)", pc->friction, pc->iceFriction), 
               (Vector2){panelX + 20, y}, 18, 0, WHITE); y += lineH;

    y += 8.0f;

    // Water
    DrawTextEx(lobby_fonts[FONT20], "WATER", 
               (Vector2){panelX + 20, y}, 20, 0, SKYBLUE); 
    y += lineH + 8.0f;

    DrawTextEx(lobby_fonts[FONT18], TextFormat("Buoyancy     : %.1f", pc->waterBuoyancy), 
               (Vector2){panelX + 20, y}, 18, 0, WHITE); y += lineH;
    DrawTextEx(lobby_fonts[FONT18], TextFormat("Default Sink : %.1f   (S: %.1f)", 
               pc->waterDefaultSink, pc->waterSinkWithS), 
               (Vector2){panelX + 20, y}, 18, 0, WHITE); y += lineH;
    DrawTextEx(lobby_fonts[FONT18], TextFormat("Horiz Drag   : %.3f   Vert Drag: %.3f", 
               pc->waterHorizDrag, pc->waterVertDrag), 
               (Vector2){panelX + 20, y}, 18, 0, WHITE); y += lineH;
    DrawTextEx(lobby_fonts[FONT18], TextFormat("Target Sub   : %.2f   Max Sub: %.2f", 
               pc->waterTargetSubmersion, pc->waterMaxSubmersion), 
               (Vector2){panelX + 20, y}, 18, 0, WHITE); y += lineH;

    DrawTextEx(lobby_fonts[FONT18], TextFormat("Can Jump: %s   Infinite: %s   Always Float: %s", 
               pc->waterCanJump ? "YES" : "NO",
               pc->waterInfiniteJump ? "YES" : "NO",
               pc->waterAlwaysFloat ? "YES" : "NO"), 
               (Vector2){panelX + 20, y}, 18, 0, WHITE); y += lineH;

    y += 12.0f;

    // ── Live editable constants for current skin ─────────────────────────────
    DrawTextEx(lobby_fonts[FONT20], "EDITABLE (click value)", 
               (Vector2){panelX + 20, y}, 20, 0, YELLOW); 
    y += lineH + 8.0f;

    const char* constNames[] = {
        "gravity", "moveSpeed", "jumpForce", "friction", "iceFriction",
        "waterBuoyancy", "waterDefaultSink", "waterSinkWithS",
        "waterHorizDrag", "waterVertDrag",
        "waterTargetSubmersion", "waterMaxSubmersion"
    };

    const f32* constValues[] = {
        &pc->gravity, &pc->moveSpeed, &pc->jumpForce,
        &pc->friction, &pc->iceFriction,
        &pc->waterBuoyancy, &pc->waterDefaultSink, &pc->waterSinkWithS,
        &pc->waterHorizDrag, &pc->waterVertDrag,
        &pc->waterTargetSubmersion, &pc->waterMaxSubmersion
    };

    const u32 numConstants = ARRAY_LEN(constNames);

    for (u32 i = 0; i < numConstants; ++i) {
        bool isEditingThis = (game->physicsPanelEditIndex == (s32)i);

        DrawTextEx(lobby_fonts[FONT18], TextFormat("%-20s : ", constNames[i]), 
                   (Vector2){panelX + 20, y}, 18, 0, LIGHTGRAY); 

        const char* displayValue = isEditingThis 
            ? game->physicsPanelEditBuffer 
            : TextFormat("%.3f", *constValues[i]);

        DrawTextEx(lobby_fonts[FONT18], displayValue, 
                   (Vector2){panelX + 220, y}, 18, 0, isEditingThis ? YELLOW : WHITE); 

        Rectangle valueRect = {panelX + 210, y, 130, lineH};
        DrawRectangleLinesEx(valueRect, 1, RED);

        if (!isEditingThis && IsMouseButtonPressed(MOUSE_BUTTON_LEFT) &&
            CheckCollisionPointRec(GetMousePosition(), valueRect)) {
            snprintf(game->physicsPanelEditBuffer, sizeof(game->physicsPanelEditBuffer), "%.3f", *constValues[i]);
            game->physicsPanelEditCursor = (u32)strlen(game->physicsPanelEditBuffer);
            game->physicsPanelEditIndex = (s32)i;
        }

        y += lineH;
    }

    EndScissorMode();

    // Help text
    DrawTextEx(lobby_fonts[FONT18], "F2 - Hide   Wheel - Scroll   Click value to edit", 
               (Vector2){panelX + 20, panelY + panelH - 35}, 18, 0, GRAY);
}

void updatePhysicsDebugPanel(LobbyGame_St* const game) {
    if (!showPhysicsDebugPanel || game->physicsPanelEditIndex == -1) return;

    int key = GetCharPressed();
    while (key > 0) {
        if ((key >= '0' && key <= '9') || key == '.' || key == '-') {
            if (game->physicsPanelEditCursor < sizeof(game->physicsPanelEditBuffer) - 1) {
                game->physicsPanelEditBuffer[game->physicsPanelEditCursor++] = (char)key;
                game->physicsPanelEditBuffer[game->physicsPanelEditCursor] = '\0';
            }
        }
        key = GetCharPressed();
    }

    if (IsKeyPressed(KEY_BACKSPACE) || IsKeyPressedRepeat(KEY_BACKSPACE)) {
        if (game->physicsPanelEditCursor > 0) {
            game->physicsPanelEditCursor--;
            game->physicsPanelEditBuffer[game->physicsPanelEditCursor] = '\0';
        }
    }

    if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_KP_ENTER)) {
        f32 newVal = (f32)atof(game->physicsPanelEditBuffer);
        PhysicsConstants_St* pc = &game->physics[game->player.textureId];

        switch (game->physicsPanelEditIndex) {
            case 0:  pc->gravity             = newVal; break;
            case 1:  pc->moveSpeed           = newVal; break;
            case 2:  pc->jumpForce           = newVal; break;
            case 3:  pc->friction            = newVal; break;
            case 4:  pc->iceFriction         = newVal; break;
            case 5:  pc->waterBuoyancy       = newVal; break;
            case 6:  pc->waterDefaultSink    = newVal; break;
            case 7:  pc->waterSinkWithS      = newVal; break;
            case 8:  pc->waterHorizDrag      = newVal; break;
            case 9:  pc->waterVertDrag       = newVal; break;
            case 10: pc->waterTargetSubmersion = newVal; break;
            case 11: pc->waterMaxSubmersion  = newVal; break;
        }
        game->physicsPanelEditIndex = -1;   // commit
    }

    if (IsKeyPressed(KEY_ESCAPE)) {
        game->physicsPanelEditIndex = -1;   // cancel
    }
}

void updateUIOnResize(void) {
    const f32 w = (f32)systemSettings.video.width;
    const f32 h = (f32)systemSettings.video.height;

    // ── Skin button (top-right corner, fixed size) ─────────────────────
    skinButtonRect.x      = w - 70.0f;
    skinButtonRect.y      = (h - skinButtonRect.height) / 2.0f;
}