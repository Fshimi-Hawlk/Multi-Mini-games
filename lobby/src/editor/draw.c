/**
    @file editor/draw.c
    @author Fshimi-Hawlk
    @date 2026-03-30
    @date 2026-04-11
    @brief Implementation of the level editor drawing functions.
*/

#include "ui/game.h"

#include "editor/editor.h"
#include "editor/types.h"
#include "editor/properties.h"

#include "utils/globals.h"

#include "sharedUtils/geometry.h"

#include "sharedWidgets/button.h"
#include "sharedWidgets/textBox.h"
#include "sharedWidgets/dropdown.h"
#include "sharedWidgets/scrollFrame.h"

static const char* terrainKindNames[] = {
    "NORMAL", "GRASS", "WOOD PLANK", "STONE", "ICE", "BOUNCY", "MOV_H", "MOV_V", "WATER", "DECOR", "PORTAL"
};

void drawEditor(const LobbyGame_St* const game) {
    if (!game->editorMode) return;

    // Draw grid
    if (game->showGrid) {
        float step = game->gridStep;
        Vector2 topLeft = GetScreenToWorld2D((Vector2){0, 0}, game->cam);
        Vector2 bottomRight = GetScreenToWorld2D((Vector2){(float)GetScreenWidth(), (float)GetScreenHeight()}, game->cam);
        
        for (float x = floorf(topLeft.x / step) * step; x < bottomRight.x; x += step)
            DrawLineV((Vector2){x, topLeft.y}, (Vector2){x, bottomRight.y}, Fade(GRAY, 0.2f));
        for (float y = floorf(topLeft.y / step) * step; y < bottomRight.y; y += step)
            DrawLineV((Vector2){topLeft.x, y}, (Vector2){bottomRight.x, y}, Fade(GRAY, 0.2f));
    }

    // Draw world entities
    BeginMode2D(game->cam); {
        lobby_drawTerrains();

        for (u32 i = 0; i < terrains.count; i++) {
            LobbyTerrain_St* t = &terrains.items[i];
            // Highlight selected
            bool isSelected = false;
            for (u32 s = 0; s < selectedIndices.count; s++) {
                if (selectedIndices.items[s] == (s32)i) { isSelected = true; break; }
            }
            if (isSelected) DrawRectangleLinesEx(t->rect, 3.0f, YELLOW);
            
            if (t->kind == TERRAIN_KIND_PORTAL) {
                Vector2 center = getRectCenterPos(t->rect);
                DrawLineV(center, t->portalTargetPosition, Fade(PURPLE, 0.5f));
                DrawCircleV(t->portalTargetPosition, 10, PURPLE);
            }
        }

        if (editorDragMode == DRAG_PLACING_NEW) {
            DrawRectangleLinesEx(dragPreviewRect, 2.0f, GREEN);
        } else if (editorDragMode == DRAG_MULTI_SELECT) {
            DrawRectangleLinesEx(multiSelectRect, 1.0f, SKYBLUE);
            DrawRectangleRec(multiSelectRect, Fade(SKYBLUE, 0.2f));
        }
    }
    EndMode2D();

    // UI Overlays
    DrawRectangle(0, 0, 250, GetScreenHeight(), Fade(BLACK, 0.8f));
    DrawText("EDITOR MODE", 20, 20, 20, GOLD);

    textButtonDraw(&btnLoad, lobby_fonts[FONT24], 20);
    textButtonDraw(&btnSave, lobby_fonts[FONT24], 20);
    textButtonDraw(&btnGenerate, lobby_fonts[FONT24], 20);

    // Palette
    scrollFrameBegin(&paletteScroll);
    for (int i = 0; i < __terrainKindCount; i++) {
        Rectangle r = { 20, 100 + i * 40 - paletteScroll.scroll.y, 210, 30 };
        if (CheckCollisionPointRec(GetMousePosition(), r)) {
            DrawRectangleRec(r, Fade(WHITE, 0.2f));
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) currentPaletteKind = (TerrainKind_Et)i;
        }
        if (currentPaletteKind == (TerrainKind_Et)i) DrawRectangleLinesEx(r, 2, GOLD);
        DrawText(terrainKindNames[i], (int)r.x + 10, (int)r.y + 5, 18, WHITE);
    }
    scrollFrameEnd();

    // Properties
    if (game->showPropertiesPanel) {
        DrawRectangle(GetScreenWidth() - 300, 0, 300, GetScreenHeight(), Fade(BLACK, 0.8f));
        DrawText("PROPERTIES", GetScreenWidth() - 280, 20, 20, SKYBLUE);
        
        scrollFrameBegin(&propertiesScroll);
        float y = 60 - propertiesScroll.scroll.y;
        
        if (focusedTerrainIndex != -1) {
            DrawText(TextFormat("ID: %d", focusedTerrainIndex), GetScreenWidth() - 280, y, 18, WHITE);
            y += 30;
            textBoxDraw(&tbPosX, lobby_fonts[FONT24], 18);
            textBoxDraw(&tbPosY, lobby_fonts[FONT24], 18);
            y += 80;
            dropdownDraw(&dropdownType, lobby_fonts[FONT24], 18);
        } else {
            DrawText("Multiple Selection", GetScreenWidth() - 280, y, 18, GRAY);
        }
        scrollFrameEnd();
    }
}
