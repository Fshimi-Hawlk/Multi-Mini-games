/**
    @file ui/editorUtils.c
    @author Grok (assisted)
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
    t.color = (Color){139, 69, 19, 255};   // brown
    t.roundness = 0.0f;
    t.type = type;
    return t;
}

bool terrainsOverlap(const LobbyTerrain_St* const a, const LobbyTerrain_St* const b) {
    return CheckCollisionRecs(a->rect, b->rect);
}