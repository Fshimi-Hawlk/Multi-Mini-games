/**
    @file ui/game.c
    @author Fshimi-Hawlk
    @author LeandreB8
    @author i-Charlys
    @date 2026-01-30
    @date 2026-04-02
    @brief Optimized drawing routines for lobby gameplay elements with rich visuals.
*/

#include "core/game.h"
#include "ui/game.h"
#include "utils/utils.h"
#include "utils/globals.h"
#include <math.h>

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

void drawLobbyBackground(void) {
    // Draw a rich sky gradient
    DrawRectangleGradientV(0, 0, GetScreenWidth(), GetScreenHeight(), (Color){135, 206, 235, 255}, (Color){255, 255, 255, 255});
    
    // Draw some distant decorative mountains (simplified)
    DrawTriangle((Vector2){-1000, 400}, (Vector2){0, 100}, (Vector2){1000, 400}, Fade(DARKGRAY, 0.2f));
    DrawTriangle((Vector2){0, 400}, (Vector2){1000, 50}, (Vector2){2000, 400}, Fade(DARKGRAY, 0.3f));
}

void drawLobbyTerrains(void) {
    // FPS Optimization: Pre-calculate some values
    float time = (float)GetTime();
    
    for (u32 i = 0; i < terrains.count; i++) {
        const LobbyTerrain_St* t = &terrains.items[i];
        
        // Visibility check (basic culling)
        // Note: For simplicity we assume they are somewhat near the player
        
        switch (t->type) {
            case TERRAIN_WATER: {
                Color waterColor = (Color){ 30, 120, 250, 140 };
                DrawRectangleRec(t->rect, waterColor);
                // Simple wave effect (no Rounded call)
                float wave = sinf(time * 2.0f + t->rect.x * 0.01f) * 4.0f;
                DrawLineEx((Vector2){t->rect.x, t->rect.y + wave}, (Vector2){t->rect.x + t->rect.width, t->rect.y + wave}, 3.0f, (Color){150, 220, 255, 200});
            } break;

            case TERRAIN_BOUNCY:
                DrawRectangleRec(t->rect, (Color){255, 50, 255, 255});
                DrawRectangleLinesEx(t->rect, 2.0f, WHITE);
                break;

            case TERRAIN_ICE:
                DrawRectangleRec(t->rect, (Color){200, 240, 255, 255});
                DrawRectangleLinesEx(t->rect, 1.0f, Fade(WHITE, 0.5f));
                break;

            case TERRAIN_PORTAL: {
                float pulse = (sinf(time * 5.0f) + 1.0f) * 0.5f;
                DrawRectangleRec(t->rect, (Color){100, 0, 200, 200});
                DrawRectangleLinesEx(t->rect, 2.0f + pulse * 3.0f, PURPLE);
                DrawCircleV(getRectCenterPos(t->rect), 5 + pulse * 10, WHITE);
            } break;

            case TERRAIN_WOOD:
                DrawRectangleRec(t->rect, (Color){100, 60, 20, 255});
                DrawRectangleLinesEx(t->rect, 1.0f, (Color){60, 30, 10, 255});
                break;

            case TERRAIN_STONE:
                DrawRectangleRec(t->rect, (Color){120, 120, 120, 255});
                DrawRectangleLinesEx(t->rect, 1.5f, DARKGRAY);
                break;

            case TERRAIN_DECORATIVE:
                // Use Rounded ONLY for decoration to save FPS on static platforms
                DrawRectangleRounded(t->rect, t->roundness, 6, t->color);
                break;

            default: // TERRAIN_NORMAL
                DrawRectangleRec(t->rect, t->color);
                DrawRectangleLinesEx(t->rect, 1.0f, Fade(BLACK, 0.2f));
                break;
        }
    }

    // Interaction zones (Rich visuals)
    for (u32 zoneIndex = 0; zoneIndex < __miniGameCount; zoneIndex++) {
        if (zoneIndex == MINI_GAME_LOBBY) continue;

        GameInteractionZone_St currentZone = gameInteractionZones[zoneIndex];
        if (currentZone.hitbox.width == 0) continue; 

        // Glowing effect for game zones
        float glow = (sinf(time * 3.0f) + 1.0f) * 0.5f;
        Color zoneColor = (Color){255, 215, 0, 100}; // Always Gold
        Color edgeColor = GOLD;

        DrawRectangleRounded(currentZone.hitbox, 0.3f, 8, zoneColor);
        DrawRectangleRoundedLinesEx(currentZone.hitbox, 0.3f, 8, 2.0f + glow * 2.0f, edgeColor);

        const char* gameName = currentZone.name;
        int fontSize = 22;
        float textX = currentZone.hitbox.x + (currentZone.hitbox.width - MeasureText(gameName, fontSize)) / 2.0f;
        float textY = currentZone.hitbox.y - 30.0f - glow * 5.0f;
        DrawText(gameName, (int)textX, (int)textY, fontSize, edgeColor);
    }
}
