/**
    @file ui/game.c
    @author Maxime CHAUVEAU
    @date 2026-02-25
    @brief Low-level drawing routines for lobby gameplay elements.

    This file contains primitive rendering functions used to visualize:
        - the player character (either debug circle or textured sprite)
        - static platforms in the lobby world
        - game zones (mini-game triggers) with visual feedback

    These functions are meant to be called during the main render pass after
    the camera has been set up. They operate directly on Raylib drawing primitives
    and expect world-space coordinates.

    All functions in this file are stateless with respect to side-effects outside
    of Raylib's drawing context - they only read game state and issue draw calls.

    @see `core/game.h` for `getPlayerCollisionBox` and `getPlayerCenter`
    @see `utils/utils.h` for `getTextureRec`
*/

#include "ui/game.h"
#include "core/game.h"
#include "utils/utils.h"

void drawPlayer(const LobbyGame_St* const game, const Player_st* const player) {
    if (player->textureId == PLAYER_TEXTURE_DEFAULT) {
        DrawCircleV(player->position, player->radius, BLUE);
    }
    else {
        DrawTexturePro(
            game->playerVisuals.textures[player->textureId],
            getTextureRec(game->playerVisuals.textures[player->textureId]),
            getPlayerCollisionBox(player),
            getPlayerCenter(player),
            player->angle,
            WHITE
        );
    }
}

void drawPlatforms(const Platform_st* const platforms, const int nbPlatforms) {
    for (int i = 0; i < nbPlatforms; i++) {
        DrawRectangleRounded(platforms[i].rect, platforms[i].roundness, 0, platforms[i].color);
    }
}

void drawGameZones(const GameZone_st* const zones, const u32 count) {
    for (u32 i = 0; i < count; ++i) {
        Color zoneColor = zones[i].debugColor;
        
        // Draw semi-transparent hitbox
        Color fillColor = (Color){ zoneColor.r, zoneColor.g, zoneColor.b, 100 };
        DrawRectangleRec(zones[i].hitbox, fillColor);
        
        // Draw border
        DrawRectangleLinesEx(zones[i].hitbox, 2, zoneColor);
        
        // Draw label
        const char* label = "BOWLING";
        int fontSize = 16;
        Vector2 textSize = MeasureTextEx(GetFontDefault(), label, fontSize, 1);
        Vector2 textPos = {
            zones[i].hitbox.x + (zones[i].hitbox.width - textSize.x) / 2,
            zones[i].hitbox.y + (zones[i].hitbox.height - textSize.y) / 2
        };
        DrawText(label, (int)textPos.x, (int)textPos.y, fontSize, zoneColor);
        
        // Draw interaction hint
        const char* hint = "Press E to play";
        int hintFontSize = 12;
        Vector2 hintSize = MeasureTextEx(GetFontDefault(), hint, hintFontSize, 1);
        Vector2 hintPos = {
            zones[i].hitbox.x + (zones[i].hitbox.width - hintSize.x) / 2,
            zones[i].hitbox.y + zones[i].hitbox.height + 5
        };
        DrawText(hint, (int)hintPos.x, (int)hintPos.y, hintFontSize, LIGHTGRAY);
    }
}
