/**
    @file ui/editor.c
    @author Grok (assisted)
    @date 2026-03-27
    @brief Implementation of the simple Raylib-based lobby level editor.

    Contributors:
        - Grok:
            - Initial editor mode scaffolding and UI overlay

    This is the first minimal version: only mode toggle + visual indicator.
    More features (palette, placement, selection…) will be added in subsequent steps.
*/

#include "ui/editor.h"
#include "raylib.h"
#include "ui/editorUtils.h"

#include "utils/globals.h"
#include "utils/utils.h"

#include "systemSettings.h"

static TerrainType_Et currentPaletteType = TERRAIN_NORMAL;

void toggleEditorMode(LobbyGame_St* const game) {
    if (IsKeyPressed(KEY_F1)) {
        game->editorMode = !game->editorMode;

        if (game->editorMode) {
            log_info("Entered Editor Mode - game paused");
            game->showLeftPalette = true;
        } else {
            log_info("Exited Editor Mode - returning to play");
            game->showLeftPalette = false;
            game->showPropertiesPanel = false;
        }
    }
}

void updateEditor(LobbyGame_St* const game, f32 dt) {
    UNUSED(dt);

    if (!game->editorMode) {
        return;
    }

    Vector2 mouseWorld = GetScreenToWorld2D(GetMousePosition(), game->cam);

    // ── Palette selection (simple slot click) ───────────────────────────────
    if (game->showLeftPalette && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        Vector2 mouseScreen = GetMousePosition();
        if (mouseScreen.x < 220.0f) {
            int slot = (int)((mouseScreen.y - 110.0f) / 55.0f);
            if (slot >= 0 && slot < __terrainTypeCount) {
                currentPaletteType = (TerrainType_Et)slot;
                log_info("Selected terrain type %d", slot);
            }
        }
    }

    // ── Click to place new terrain ──────────────────────────────────────────
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        // Only place if click is not on UI panels
        bool clickedOnUI = (game->showLeftPalette && GetMousePosition().x < 220.0f) ||
                           (game->showPropertiesPanel && GetMousePosition().x > systemSettings.video.width - 260.0f);

        if (!clickedOnUI) {
            s32 hit = findTerrainAtPoint(mouseWorld);
            if (hit == -1) {
                // Place new terrain
                LobbyTerrain_St newTerrain = createDefaultTerrain(currentPaletteType, mouseWorld);
                da_append(&terrains, newTerrain);
                log_info("Placed new terrain at %.1f, %.1f", mouseWorld.x, mouseWorld.y);
            } else {
                // Select existing terrain
                game->selectedTerrainIndex = hit;
                if (hit >= 0) {
                    log_info("Selected terrain index %d (type %d)", hit, terrains.items[hit].type);
                }
            }
        }
    }

    // ── Delete selected terrain ─────────────────────────────────────────────
    if (game->selectedTerrainIndex >= 0 && IsKeyPressed(KEY_DELETE)) {
        u32 idx = (u32)game->selectedTerrainIndex;
        da_remove_unordered(&terrains, idx);
        game->selectedTerrainIndex = -1;
        log_info("Deleted terrain");
    }

    // ── Camera Zoom (mouse wheel, centered on cursor) ───────────────────────
    f32 wheel = GetMouseWheelMove();
    if (wheel != 0.0f) {
        Vector2 mouseWorldBefore = GetScreenToWorld2D(GetMousePosition(), game->cam);

        game->cam.zoom += wheel * 0.1f;
        if (game->cam.zoom < 0.2f) game->cam.zoom = 0.2f;
        if (game->cam.zoom > 5.0f) game->cam.zoom = 5.0f;

        Vector2 mouseWorldAfter = GetScreenToWorld2D(GetMousePosition(), game->cam);

        // Keep zoom centered on cursor
        game->cam.target.x += mouseWorldBefore.x - mouseWorldAfter.x;
        game->cam.target.y += mouseWorldBefore.y - mouseWorldAfter.y;
    }

    // ── Camera Pan ──────────────────────────────────────────────────────────
    Vector2 pan = {0.0f, 0.0f};
    f32 panSpeed = 600.0f * dt;

    // Middle mouse drag
    if (IsMouseButtonDown(MOUSE_BUTTON_MIDDLE)) {
        Vector2 delta = GetMouseDelta();
        pan.x = -delta.x / game->cam.zoom;
        pan.y = -delta.y / game->cam.zoom;
    }

    // Keyboard pan (WASD or arrows)
    if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT))  pan.x -= panSpeed;
    if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT)) pan.x += panSpeed;
    if (IsKeyDown(KEY_W) || IsKeyDown(KEY_UP))    pan.y -= panSpeed;
    if (IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN))  pan.y += panSpeed;

    game->cam.target.x += pan.x;
    game->cam.target.y += pan.y;

    // ── Reset camera with R ─────────────────────────────────────────────────
    if (IsKeyPressed(KEY_R)) {
        game->cam.target = (Vector2){0.0f, 0.0f};
        game->cam.zoom   = 1.0f;
        log_info("Editor camera reset to default");
    }

    // ── Panel toggles ───────────────────────────────────────────────────────
    if (IsKeyPressed(KEY_Q)) {
        game->showLeftPalette = !game->showLeftPalette;
    }

    if (IsKeyPressed(KEY_P)) {
        game->showPropertiesPanel = !game->showPropertiesPanel;
    }
}

void drawEditor(const LobbyGame_St* const game) {
    if (!game->editorMode) {
        return;
    }

    BeginMode2D(game->cam); {
        // ── Visual selection highlight ──────────────────────────────────────────
        if (game->selectedTerrainIndex >= 0 && (size_t)game->selectedTerrainIndex < terrains.count) {
            const LobbyTerrain_St* selected = &terrains.items[game->selectedTerrainIndex];
            
            // Thick yellow outline
            DrawRectangleRec(selected->rect, Fade(YELLOW, 0.5));
            
            // Optional small label above the selected terrain
            DrawTextEx(lobby_fonts[FONT18],
                       TextFormat("Selected Type %d", selected->type),
                       (Vector2){selected->rect.x, selected->rect.y - 25.0f},
                       18.0f, 0.0f, YELLOW);
        }
    } EndMode2D();

    int screenW = systemSettings.video.width;
    int screenH = systemSettings.video.height;

    f32 topPanelHeight = 50.0f;
    f32 bottomPanelHeight = 30.0f;
    f32 leftPanelWidth = 220.0f;
    f32 rightPanelWidth = 260.0f;

    // ── Top Toolbar ───────────────────────────────────────────────────────
    DrawRectangle(0, 0, screenW, topPanelHeight, Fade(BLACK, 0.75f));

    // Mode indicator
    DrawTextEx(lobby_fonts[FONT20], "EDITOR MODE", (Vector2){20.0f, 12.0f}, 20.0f, 0.0f, YELLOW);

    // Placeholder buttons (Save / Load / Undo / Redo)
    DrawRectangle(300, 8, (topPanelHeight + bottomPanelHeight), 34, Fade(DARKGRAY, 0.9f));
    DrawTextEx(lobby_fonts[FONT18], "Save", (Vector2){315.0f, 14.0f}, 18.0f, 0.0f, WHITE);

    DrawRectangle(390, 8, 80, 34, Fade(DARKGRAY, 0.9f));
    DrawTextEx(lobby_fonts[FONT18], "Load", (Vector2){405.0f, 14.0f}, 18.0f, 0.0f, WHITE);

    DrawRectangle(480, 8, 80, 34, Fade(DARKGRAY, 0.9f));
    DrawTextEx(lobby_fonts[FONT18], "Undo", (Vector2){495.0f, 14.0f}, 18.0f, 0.0f, WHITE);

    DrawRectangle(570, 8, 80, 34, Fade(DARKGRAY, 0.9f));
    DrawTextEx(lobby_fonts[FONT18], "Redo", (Vector2){585.0f, 14.0f}, 18.0f, 0.0f, WHITE);

    // ── Left Sidebar: Terrain Palette ─────────────────────────────────────
    if (game->showLeftPalette) {
        DrawRectangle(0, topPanelHeight, leftPanelWidth, screenH - (topPanelHeight + bottomPanelHeight), Fade(BLACK, 0.8f));
        DrawTextEx(lobby_fonts[FONT20], "Terrain Types", (Vector2){20.0f, 65.0f}, 20.0f, 0.0f, WHITE);

        // Placeholder palette slots (will become real TerrainType_Et list)
        for (int i = 0; i < 8; ++i) {
            int y = 110 + i * 55;
            DrawRectangle(20, y, 180, 45, Fade(GRAY, 0.6f));
            DrawTextEx(lobby_fonts[FONT18], TextFormat("Type %d", i), (Vector2){35.0f, (f32)y + 12.0f}, 18.0f, 0.0f, LIGHTGRAY);
        }
    }

    // ── Right Sidebar: Properties Panel ───────────────────────────────────
    if (game->showPropertiesPanel) {
        DrawRectangle(screenW - rightPanelWidth, topPanelHeight, rightPanelWidth, screenH - (topPanelHeight + bottomPanelHeight), Fade(BLACK, 0.8f));
        DrawTextEx(lobby_fonts[FONT20], "Properties", (Vector2){(f32)screenW - rightPanelWidth + 20.0f, 65.0f}, 20.0f, 0.0f, WHITE);

        // Placeholder content
        DrawTextEx(lobby_fonts[FONT18], "No selection", (Vector2){(f32)screenW - rightPanelWidth + 20.0f, 120.0f}, 18.0f, 0.0f, GRAY);
    }

    // ── Bottom Status Bar ─────────────────────────────────────────────────
    DrawRectangle(0, screenH - bottomPanelHeight, screenW, bottomPanelHeight, Fade(BLACK, 0.75f));

    Vector2 mouseWorld = GetScreenToWorld2D(GetMousePosition(), game->cam);
    DrawTextEx(lobby_fonts[FONT18],
               TextFormat("Mouse: %.1f, %.1f   Zoom: %.2f   Terrains: %u",
                          mouseWorld.x, mouseWorld.y, game->cam.zoom, terrains.count),
               (Vector2){20.0f, (f32)screenH - 25.0f},
               18.0f, 0.0f, LIGHTGRAY);
}