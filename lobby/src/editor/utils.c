/**
    @file editor/utils.c
    @author Fshimi-Hawlk
    @date 2026-03-27
    @date 2026-04-13
    @brief Pure logic utilities used only by the level editor.
*/
#include "editor/types.h"
#include "editor/properties.h"
#include "editor/utils.h"

#include "utils/globals.h"

#include "sharedUtils/mathUtils.h"
#include "sharedUtils/geometry.h"

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

Color getTerrainKindColor(TerrainKind_Et type) {
    Color typeColor = {0};

    switch (type) {
        case TERRAIN_KIND_NORMAL:        typeColor = (Color) {139, 69, 19, 255};   break;
        case TERRAIN_KIND_WOOD_PLANK:    typeColor = (Color) {139, 69, 19, 255};   break;
        case TERRAIN_KIND_STONE:         typeColor = (Color) {160, 160, 160, 255}; break;
        case TERRAIN_KIND_ICE:           typeColor = (Color) {180, 220, 255, 255}; break;
        case TERRAIN_KIND_BOUNCY:        typeColor = (Color) {255, 100, 0, 255};   break;
        case TERRAIN_KIND_MOVING_H:
        case TERRAIN_KIND_MOVING_V:      typeColor = (Color) {70, 130, 180, 255};  break;
        case TERRAIN_KIND_WATER:         typeColor = (Color) {30, 100, 200, 180};  break;
        case TERRAIN_KIND_DECORATIVE:    typeColor = (Color) {34, 139, 34, 80};   break;
        case TERRAIN_KIND_PORTAL:        typeColor = (Color) {200, 0, 255, 180};   break;
        default:                    log_warn("TerrainKind_Et");
    }

    return typeColor;
}

LobbyTerrain_St createDefaultTerrain(TerrainKind_Et type, Vector2 position) {
    LobbyTerrain_St t = {0};
    t.rect = (Rectangle) {position.x, position.y, 200.0f, 30.0f};
    t.roundness = 0.0f;
    t.kind = type;

    t.color = getTerrainKindColor(type);
    
    return t;
}

bool terrainsOverlap(const LobbyTerrain_St* const a, const LobbyTerrain_St* const b) {
    return CheckCollisionRecs(a->rect, b->rect);
}

/* ── Editor-specific helpers ─────────────────────────────────────────────── */

Vector2 getMouseWorld(const LobbyGame_St* const game) {
    return GetScreenToWorld2D(GetMousePosition(), game->cam);
}

void fillResizeHandles(Rectangle handles[8], Rectangle r, f32 hs) {
    handles[0] = (Rectangle) {r.x - hs/2,               r.y - hs/2,               hs, hs};
    handles[1] = (Rectangle) {r.x + r.width/2 - hs/2,   r.y - hs/2,               hs, hs};
    handles[2] = (Rectangle) {r.x + r.width - hs/2,     r.y - hs/2,               hs, hs};
    handles[3] = (Rectangle) {r.x + r.width - hs/2,     r.y + r.height/2 - hs/2,  hs, hs};
    handles[4] = (Rectangle) {r.x + r.width - hs/2,     r.y + r.height - hs/2,    hs, hs};
    handles[5] = (Rectangle) {r.x + r.width/2 - hs/2,   r.y + r.height - hs/2,    hs, hs};
    handles[6] = (Rectangle) {r.x - hs/2,               r.y + r.height - hs/2,    hs, hs};
    handles[7] = (Rectangle) {r.x - hs/2,               r.y + r.height/2 - hs/2,  hs, hs};
}

Rectangle computeSelectedGroupBox(void) {
    if (selectedIndices.count == 0) {
        return (Rectangle) {0};
    }

    // Start with the first selected terrain
    s32 firstIdx = selectedIndices.items[0];
    if (firstIdx < 0 || (size_t)firstIdx >= terrains.count) {
        return (Rectangle) {0};
    }

    Rectangle groupBox = terrains.items[firstIdx].rect;

    // Expand to include all other selected terrains
    for (size_t i = 1; i < selectedIndices.count; ++i) {
        s32 idx = selectedIndices.items[i];
        if (idx < 0 || (size_t)idx >= terrains.count) continue;

        const Rectangle r = terrains.items[idx].rect;

        f32 minX = min(groupBox.x, r.x);
        f32 minY = min(groupBox.y, r.y);
        f32 maxX = max(groupBox.x + groupBox.width, r.x + r.width);
        f32 maxY = max(groupBox.y + groupBox.height, r.y + r.height);

        groupBox.x      = minX;
        groupBox.y      = minY;
        groupBox.width  = maxX - minX;
        groupBox.height = maxY - minY;
    }

    return groupBox;
}

Rectangle computeClipboardGroupBox(void) {
    Rectangle groupBox = {0};
    bool first = true;
    for (size_t i = 0; i < clipboard.count; ++i) {
        Rectangle r = clipboard.items[i].rect;
        if (first) {
            groupBox = r;
            first = false;
        } else {
            groupBox.x = min(groupBox.x, r.x);
            groupBox.y = min(groupBox.y, r.y);
            groupBox.width = max(groupBox.x + groupBox.width, r.x + r.width) - groupBox.x;
            groupBox.height = max(groupBox.y + groupBox.height, r.y + r.height) - groupBox.y;
        }
    }
    return first ? (Rectangle) {0} : groupBox;
}

Vector2 getPasteAnchorOffset(Rectangle groupBox, s32 anchorIndex) {
    f32 hw = groupBox.width / 2.0f;
    f32 hh = groupBox.height / 2.0f;

    switch (anchorIndex) {
        case 0: return (Vector2) {0.0f, 0.0f};
        case 1: return (Vector2) {hw, 0.0f};
        case 2: return (Vector2) {groupBox.width, 0.0f};
        case 3: return (Vector2) {0.0f, hh};
        case 4: return (Vector2) {hw, hh};
        case 5: return (Vector2) {groupBox.width, hh};
        case 6: return (Vector2) {0.0f, groupBox.height};
        case 7: return (Vector2) {hw, groupBox.height};
        case 8: return (Vector2) {groupBox.width, groupBox.height};
        default: return (Vector2) {hw, hh};
    }
}

void moveSelectedByOffset(Vector2 offset) {
    for (size_t i = 0; i < selectedIndices.count; ++i) {
        s32 idx = selectedIndices.items[i];
        if (idx >= 0 && (size_t)idx < terrains.count) {
            terrains.items[idx].rect.x += offset.x;
            terrains.items[idx].rect.y += offset.y;
        }
    }
}

Rectangle getEditedTerrainPreviewRect(s32 idx) {
    if (idx < 0 || (size_t)idx >= terrains.count) {
        return (Rectangle) {0};
    }

    Rectangle r = terrains.items[idx].rect;

    // Live preview from text boxes when they are active
    if (tbPosX.buffer[0] && tbPosX.editMode)   r.x      = snapToGrid((f32)atof(tbPosX.buffer));
    if (tbPosY.buffer[0] && tbPosY.editMode)   r.y      = snapToGrid((f32)atof(tbPosY.buffer));
    if (tbWidth.buffer[0] && tbWidth.editMode) r.width  = max(20.0f, (f32)atof(tbWidth.buffer));
    if (tbHeight.buffer[0] && tbHeight.editMode) r.height = max(15.0f, (f32)atof(tbHeight.buffer));

    return r;
}

f32 snapToGrid(f32 v) {
    if (!propertiesGridSnapEnabled) return v;
    return roundf(v / propertiesGridStep) * propertiesGridStep;
}

void drawPortalHighlight(s32 idx, Color color, const Camera2D* cam) {
    if (idx < 0 || (size_t)idx >= terrains.count) return;
    if (terrains.items[idx].kind != TERRAIN_KIND_PORTAL) return;

    const Rectangle r = terrains.items[idx].rect;
    f32 thickness = r.width / 2 * cam->zoom;

    DrawCircleLinesV(getRectCenterPos(r), thickness, color);
}

s32 findZoneAtPoint(Vector2 point) {
    for (u8 i = 0; i < __miniGameIdCount; ++i) {
        if (i == MINI_GAME_ID_LOBBY) continue;
        if (CheckCollisionPointRec(point, gameZones[i].hitbox)) {
            return (s32)i;
        }
    }
    return -1;
}