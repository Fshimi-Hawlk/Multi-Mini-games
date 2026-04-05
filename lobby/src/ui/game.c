/**
    @file ui/game.c
    @author Fshimi-Hawlk
    @author LeandreB8
    @author i-Charlys (CAILLON Charles)
    @date 2026-01-30
    @date 2026-03-23
    @brief Low-level drawing routines for lobby gameplay elements.

    Contributors:
        - LeandreB8:
            - Provided the initial drawing logic
        - Fshimi-Hawlk:
            - Provided documentation
            - Reworked the texture logic

    This file contains primitive rendering functions used to visualize:
        - the player character (either debug circle or textured sprite)
        - static platforms in the lobby world

    These functions are meant to be called during the main render pass after
    the camera has been set up. They operate directly on Raylib drawing primitives
    and expect world-space coordinates.

    All functions in this file are stateless with respect to side-effects outside
    of Raylib's drawing context - they only read game state and issue draw calls.

    @see `core/game.h` for `getPlayerCollisionBox` and `getPlayerCenter`
    @see `utils/utils.h` for `getTextureRec`
*/

#include "core/game.h"

#include "ui/game.h"

#include "utils/globals.h"

#include "utils/geometry.h"

void drawPlayer(const PlayerVisuals_St* const playerVisuals, const Player_St* const player) {
    Texture2D texture = playerVisuals->textures[player->textureId];

    if (!IsTextureValid(texture)) {
        DrawCircleV(player->position, player->radius, BLUE);
        return;
    }

    DrawTexturePro(
        playerVisuals->textures[player->textureId],
        getTextureRec(playerVisuals->textures[player->textureId]),
        getPlayerCollisionBox(player),
        getPlayerCenter(player),
        player->angle,
        WHITE
    );
}

void drawLobbyTerrains(TerrainVec_St terrains, const GameInteractionZone_St gameInteractionZones[__miniGameCount]) {
    // Terrains
    for (u32 terrainIndex = 0; terrainIndex < terrains.count; terrainIndex++) {
        const LobbyTerrain_St* currentTerrain = &terrains.items[terrainIndex];
        DrawRectangleRounded(currentTerrain->rect, currentTerrain->roundness, 0, currentTerrain->color);
    }

    // Draw interaction zones
    for (u32 zoneIndex = 0; zoneIndex < __miniGameCount; zoneIndex++) {
        if (zoneIndex == MINI_GAME_LOBBY) continue;

        GameInteractionZone_St currentZone = gameInteractionZones[zoneIndex];

        DrawRectangleRounded(currentZone.hitbox, 0.4f, 0, (Color){255, 215, 0, 120});

        // Debug hitbox
        DrawRectangleRec(currentZone.hitbox, RED);

        // Game name text
        const char* gameName   = currentZone.name;
        f32Vector2  textSize   = MeasureTextEx(lobby_fonts[FONT32], gameName, 32, 0);
        f32Vector2  textPosition = {
            .x = currentZone.hitbox.x + (currentZone.hitbox.width - textSize.x) / 2.0f,
            .y = currentZone.hitbox.y - 15.0f - textSize.y / 2.0f
        };

        DrawTextEx(lobby_fonts[FONT32], gameName, textPosition, 32, 0, BLACK);
    }
}