/**
    @file ui/editorUtils.c
    @author Fshimi-Hawlk
    @date 2026-03-27
    @brief Pure logic utilities for the level editor.
*/

#include "ui/editorUtils.h"
#include "utils/globals.h"

bool pointInTerrain(const LobbyTerrain_St* const terrain, Vector2 point) {
    return CheckCollisionPointRec(point, terrain->rect);
}

s32 findTerrainAtPoint(Vector2 point) {
    for (size_t i = 0; i < terrains.count; ++i) {
        if (pointInTerrain(&terrains.items[i], point)) {
            return (s32)i;
        }
    }

    return -1;
}

LobbyTerrain_St createDefaultTerrain(TerrainType_Et type, Vector2 position) {
    LobbyTerrain_St t = {0};
    t.rect = (Rectangle){position.x, position.y, 200.0f, 30.0f};
    t.roundness = 0.0f;
    t.type = type;

    // Proper per-type colors (matching the initial level in setups/game.c)
    switch (type) {
        case TERRAIN_NORMAL:    t.color = (Color){139, 69, 19, 255}; break;
        case TERRAIN_WOOD:      t.color = (Color){139, 69, 19, 255}; t.roundness = 0.3f; break;
        case TERRAIN_STONE:     t.color = (Color){160, 160, 160, 255}; break;
        case TERRAIN_ICE:       t.color = (Color){180, 220, 255, 255}; break;
        case TERRAIN_BOUNCY:    t.color = (Color){255, 100, 0, 255}; break;
        case TERRAIN_MOVING_H:
        case TERRAIN_MOVING_V:  t.color = (Color){70, 130, 180, 255}; break;
        case TERRAIN_WATER:     t.color = (Color){30, 100, 200, 180}; t.roundness = 0.8f; break;
        case TERRAIN_DECORATIVE:t.color = (Color){34, 139, 34, 255}; break;
        case TERRAIN_PORTAL:    t.color = (Color){200, 0, 255, 180}; t.roundness = 1.0f; break;
        default:                t.color = (Color){139, 69, 19, 255}; break;
    }
    return t;
}

bool terrainsOverlap(const LobbyTerrain_St* const a, const LobbyTerrain_St* const b) {
    return CheckCollisionRecs(a->rect, b->rect);
}