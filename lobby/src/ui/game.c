/**
    @file ui/game.c
    @author Fshimi-Hawlk
    @author LeandreB8
    @author i-Charlys
    @date 2026-01-30
    @date 2026-03-23
    @brief Low-level drawing routines for lobby gameplay elements.
*/

#include "core/game.h"
#include "ui/game.h"
#include "utils/utils.h"
#include "utils/globals.h"

void drawPlayer(const PlayerVisuals_St* const playerVisuals, const Player_St* const player) {
    if (!player->active) return;

    Texture2D texture = playerVisuals->textures[player->textureId];

    if (!IsTextureValid(texture)) {
        DrawCircleV(player->position, player->radius, BLUE);
    } else {
        DrawTexturePro(
            texture,
            getTextureRec(texture),
            getPlayerCollisionBox(player),
            getPlayerCenter(player),
            player->angle,
            WHITE
        );
    }

    // Draw player name above head
    int fontSize = 16;
    float textX = player->position.x - MeasureText(player->name, fontSize) / 2.0f;
    float textY = player->position.y - player->radius - 20.0f;
    DrawText(player->name, (int)textX, (int)textY, fontSize, DARKGRAY);
}

void drawLobbyTerrains(TerrainVec_St terrains, const GameInteractionZone_St gameInteractionZones[__miniGameCount]) {
    // Terrains
    for (u32 terrainIndex = 0; terrainIndex < terrains.count; terrainIndex++) {
        const LobbyTerrain_St* currentTerrain = &terrains.items[terrainIndex];
        DrawRectangleRounded(currentTerrain->rect, currentTerrain->roundness, 0, currentTerrain->color);
    }

    // Interaction zones
    for (u32 zoneIndex = 0; zoneIndex < __miniGameCount; zoneIndex++) {
        if (zoneIndex == MINI_GAME_LOBBY) continue;

        GameInteractionZone_St currentZone = gameInteractionZones[zoneIndex];
        if (currentZone.hitbox.width == 0) continue; // Skip unused zones

        // Base visual for the zone
        DrawRectangleRounded(currentZone.hitbox, 0.4f, 0, (Color){255, 215, 0, 120});
        DrawRectangleLinesEx(currentZone.hitbox, 2, GOLD);

        // Game name text
        const char* gameName = currentZone.name;
        int fontSize = 20;
        float textX = currentZone.hitbox.x + (currentZone.hitbox.width - MeasureText(gameName, fontSize)) / 2.0f;
        float textY = currentZone.hitbox.y - 25.0f;
        DrawText(gameName, (int)textX, (int)textY, fontSize, GOLD);
    }
}
