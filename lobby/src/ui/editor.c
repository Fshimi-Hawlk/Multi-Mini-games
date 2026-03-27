/**
    @file ui/editor.c
    @author Fshimi-Hawlk
    @date 2026-03-27
    @date 2026-03-27
    @brief Implementation of the Raylib-based lobby level editor.

    Contributors:
        - Fshimi-Hawlk:
            - Overall editor structure and integration
            - Drag-to-place new terrain
            - 8-handle resize (corners + edges)
            - Drag-to-move selected terrain body
            - Click-to-deselect
            - Multi-select support with proper dynamic storage
            - Group bounding box
            - Bulk move / delete / duplicate
            - Palette highlight + terrain name strings
            - Camera reset to player
            - Ctrl+wheel pan speed control
            - H help panel
            - Ctrl+C / Ctrl+V with anchor diagram
            - Fixed multi-select drag start on Shift+drag (empty space)
            - Fixed paste anchor diagram (static screen-space, adapts to properties panel)
            - Hover info for selected terrains (outline + mouse-locked, no static text in multi-select)
            - Group bounding box robustness on shift-click multi-select
            - Scrolling frames for help panel, terrain palette and properties panel
            - Refactored to extract helper functions (resize handles, group bounding box, paste anchor offset, selected move/delete) to eliminate duplication

    @see `ui/editor.h`
*/

#include "ui/editor.h"
#include "ui/editorUtils.h"

#include "utils/globals.h"
#include "utils/utils.h"

#include "systemSettings.h"

static TerrainType_Et currentPaletteType = TERRAIN_NORMAL;

static const char* terrainTypeNames[__terrainTypeCount] = {
    "Normal",
    "Wood",
    "Stone",
    "Ice",
    "Bouncy",
    "Moving H",
    "Moving V",
    "Water",
    "Decorative",
    "Portal"
};

// Dynamic array of selected terrain indices
typeDA(s32, SelectedIndices_St);
static SelectedIndices_St selectedIndices = {0};

// Clipboard for copy/paste
typeDA(LobbyTerrain_St, Clipboard_St);
static Clipboard_St clipboard = {0};
static bool clipboardHasData = false;
static s32 pasteAnchorIndex = 4;   // default = center (index 4 in 3x3 grid)

typedef enum {
    DRAG_NONE,
    DRAG_PLACING_NEW,
    DRAG_MOVING,
    DRAG_RESIZING,
    DRAG_MULTI_SELECT
} EditorDragMode_Et;

typedef enum {
    HANDLE_NONE = 0,
    HANDLE_TOP_LEFT,
    HANDLE_TOP,
    HANDLE_TOP_RIGHT,
    HANDLE_RIGHT,
    HANDLE_BOTTOM_RIGHT,
    HANDLE_BOTTOM,
    HANDLE_BOTTOM_LEFT,
    HANDLE_LEFT
} ResizeHandle_Et;

static EditorDragMode_Et editorDragMode = DRAG_NONE;
static ResizeHandle_Et activeHandle = HANDLE_NONE;
static Vector2 dragStartWorld = {0.0f, 0.0f};
static Rectangle dragPreviewRect = {0};
static Rectangle multiSelectRect = {0};
static bool showSelectionBox = true;
static bool duplicationOffsetEnabled = true;
static f32 panMultiplier = 1.0f;
static bool showHelpPanel = false;

// Scroll offsets for the three scrollable UI panels
static f32 paletteScrollY = 0.0f;
static f32 helpScrollY = 0.0f;
static f32 propertiesScrollY = 0.0f;

// Backup of camera when entering editor so we can restore on exit
static Camera2D editorCameraBackup = {0};

/* ────────────────────────────────────────────────────────────────────────── */
/* Helper functions (extracted to eliminate duplication)                     */
/* ────────────────────────────────────────────────────────────────────────── */

static Vector2 getMouseWorld(const LobbyGame_St* const game) {
    return GetScreenToWorld2D(GetMousePosition(), game->cam);
}

static void fillResizeHandles(Rectangle handles[8], Rectangle r, f32 hs) {
    handles[0] = (Rectangle){r.x - hs/2,               r.y - hs/2,               hs, hs};
    handles[1] = (Rectangle){r.x + r.width/2 - hs/2,   r.y - hs/2,               hs, hs};
    handles[2] = (Rectangle){r.x + r.width - hs/2,     r.y - hs/2,               hs, hs};
    handles[3] = (Rectangle){r.x + r.width - hs/2,     r.y + r.height/2 - hs/2,  hs, hs};
    handles[4] = (Rectangle){r.x + r.width - hs/2,     r.y + r.height - hs/2,    hs, hs};
    handles[5] = (Rectangle){r.x + r.width/2 - hs/2,   r.y + r.height - hs/2,    hs, hs};
    handles[6] = (Rectangle){r.x - hs/2,               r.y + r.height - hs/2,    hs, hs};
    handles[7] = (Rectangle){r.x - hs/2,               r.y + r.height/2 - hs/2,  hs, hs};
}

static Rectangle computeSelectedGroupBox(void) {
    Rectangle groupBox = {0};
    bool first = true;
    for (size_t i = 0; i < selectedIndices.count; ++i) {
        s32 idx = selectedIndices.items[i];
        if (idx >= 0 && (size_t)idx < terrains.count) {
            Rectangle r = terrains.items[idx].rect;
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
    }
    return first ? (Rectangle){0} : groupBox;
}

static Rectangle computeClipboardGroupBox(void) {
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
    return first ? (Rectangle){0} : groupBox;
}

static Vector2 getPasteAnchorOffset(Rectangle groupBox, s32 anchorIndex) {
    f32 hw = groupBox.width / 2.0f;
    f32 hh = groupBox.height / 2.0f;
    switch (anchorIndex) {
        case 0: return (Vector2){0.0f, 0.0f};
        case 1: return (Vector2){hw, 0.0f};
        case 2: return (Vector2){groupBox.width, 0.0f};
        case 3: return (Vector2){0.0f, hh};
        case 4: return (Vector2){hw, hh};
        case 5: return (Vector2){groupBox.width, hh};
        case 6: return (Vector2){0.0f, groupBox.height};
        case 7: return (Vector2){hw, groupBox.height};
        case 8: return (Vector2){groupBox.width, groupBox.height};
        default: return (Vector2){hw, hh};
    }
}

static void moveSelectedByOffset(Vector2 offset) {
    for (size_t i = 0; i < selectedIndices.count; ++i) {
        s32 idx = selectedIndices.items[i];
        if (idx >= 0 && (size_t)idx < terrains.count) {
            terrains.items[idx].rect.x += offset.x;
            terrains.items[idx].rect.y += offset.y;
        }
    }
}

static void deleteSelected(LobbyGame_St* const game) {
    if (selectedIndices.count > 0) {
        for (s32 i = (s32)selectedIndices.count - 1; i >= 0; --i) {
            u32 idx = (u32)selectedIndices.items[i];
            if (idx < terrains.count) {
                da_remove_unordered(&terrains, idx);
            }
        }
    } else if (game->selectedTerrainIndex >= 0) {  // note: game is passed to callers
        u32 idx = (u32)game->selectedTerrainIndex; // (game is parameter in updateEditor)
        da_remove_unordered(&terrains, idx);
    }
    game->selectedTerrainIndex = -1;
    da_clear(&selectedIndices);
    editorDragMode = DRAG_NONE;
    activeHandle = HANDLE_NONE;
}

void toggleEditorMode(LobbyGame_St* const game) {
    if (IsKeyPressed(KEY_F1)) {
        if (!game->editorMode) {
            // Entering editor - save current camera
            editorCameraBackup = game->cam;
            game->showLeftPalette = true;
        } else {
            // Exiting editor - restore original play-mode camera (zoom 1.0 + player follow)
            game->cam.zoom = 1.0f;
            game->cam.target = game->player.position;
            game->showLeftPalette = false;
            game->showPropertiesPanel = false;
        }
        game->editorMode = !game->editorMode;
    }
}

void updateEditor(LobbyGame_St* const game, f32 dt) {
    UNUSED(dt);

    if (!game->editorMode) {
        return;
    }

    Vector2 mouseScreen = GetMousePosition();
    Vector2 mouseWorld = getMouseWorld(game);

    // Palette selection (now scroll-aware)
    if (game->showLeftPalette && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        if (mouseScreen.x < 220.0f) {
            f32 slotBaseY = 110.0f;
            f32 logicalY = mouseScreen.y - slotBaseY - paletteScrollY;
            s32 slot = (s32)(logicalY / 55.0f);
            if (slot >= 0 && slot < __terrainTypeCount) {
                currentPaletteType = (TerrainType_Et)slot;
            }
        }
    }

    // ── Mouse button pressed / drag / release logic ───────────────────────
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        bool clickedOnUI = (game->showLeftPalette && mouseScreen.x < 220.0f) ||
                           (game->showPropertiesPanel && mouseScreen.x > systemSettings.video.width - 260.0f);

        if (clickedOnUI) {
            return;
        }

        bool shiftDown = IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT);

        if (shiftDown) {
            s32 hit = findTerrainAtPoint(mouseWorld);
            if (hit != -1) {
                // Shift+click: toggle terrain in/out of multi-selection
                bool already = false;
                for (size_t i = 0; i < selectedIndices.count; ++i) {
                    if (selectedIndices.items[i] == hit) {
                        da_remove_unordered(&selectedIndices, i);
                        already = true;
                        break;
                    }
                }
                if (!already) {
                    da_append(&selectedIndices, hit);
                }
                if (selectedIndices.count > 0) {
                    game->selectedTerrainIndex = da_last(&selectedIndices);
                } else {
                    game->selectedTerrainIndex = -1;
                }
            } else {
                // Shift + drag on empty space starts multi-select box
                editorDragMode = DRAG_MULTI_SELECT;
                dragStartWorld = mouseWorld;
                multiSelectRect = (Rectangle) {mouseWorld.x, mouseWorld.y, 0.0f, 0.0f};
            }
        } else {
            if (game->selectedTerrainIndex >= 0) {
                // Check handles on currently selected terrain first
                const LobbyTerrain_St* selected = &terrains.items[game->selectedTerrainIndex];
                const f32 hs = 8.0f / game->cam.zoom;

                Rectangle r = selected->rect;
                Rectangle handles[8];
                fillResizeHandles(handles, r, hs);

                activeHandle = HANDLE_NONE;
                for (s32 i = 0; i < 8; ++i) {
                    if (CheckCollisionPointRec(mouseWorld, handles[i])) {
                        activeHandle = (ResizeHandle_Et)(i + 1);
                        editorDragMode = DRAG_RESIZING;
                        dragStartWorld = mouseWorld;
                        dragPreviewRect = selected->rect;
                        break;
                    }
                }

                if (activeHandle != HANDLE_NONE) {
                    // handle hit - proceed
                } else if (CheckCollisionPointRec(mouseWorld, selected->rect)) {
                    editorDragMode = DRAG_MOVING;
                    dragStartWorld = mouseWorld;
                    dragPreviewRect = selected->rect;
                } else {
                    // clicked outside -> deselect everything
                    game->selectedTerrainIndex = -1;
                    da_clear(&selectedIndices);
                    editorDragMode = DRAG_NONE;
                    activeHandle = HANDLE_NONE;
                }
            } else {
                s32 hit = findTerrainAtPoint(mouseWorld);

                if (hit == -1) {
                    editorDragMode = DRAG_PLACING_NEW;
                    dragStartWorld = mouseWorld;
                    dragPreviewRect = (Rectangle) {mouseWorld.x, mouseWorld.y, 0.0f, 0.0f};
                } else {
                    // direct single select
                    game->selectedTerrainIndex = hit;
                    da_clear(&selectedIndices);
                    da_append(&selectedIndices, hit);
                    editorDragMode = DRAG_NONE;
                    activeHandle = HANDLE_NONE;
                }
            }
        }
    }

    // ── Live drag ─────────────────────────────────────────────────────────
    if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
        if (editorDragMode == DRAG_PLACING_NEW) {
            dragPreviewRect.x = min(dragStartWorld.x, mouseWorld.x);
            dragPreviewRect.y = min(dragStartWorld.y, mouseWorld.y);
            dragPreviewRect.width = fabsf(mouseWorld.x - dragStartWorld.x);
            dragPreviewRect.height = fabsf(mouseWorld.y - dragStartWorld.y);
        } else if (editorDragMode == DRAG_MOVING && game->selectedTerrainIndex >= 0) {
            Vector2 offset = {mouseWorld.x - dragStartWorld.x, mouseWorld.y - dragStartWorld.y};
            dragPreviewRect.x += offset.x;
            dragPreviewRect.y += offset.y;
            dragStartWorld = mouseWorld;
        } else if (editorDragMode == DRAG_RESIZING && activeHandle != HANDLE_NONE && game->selectedTerrainIndex >= 0) {
            const LobbyTerrain_St* original = &terrains.items[game->selectedTerrainIndex];
            Rectangle r = original->rect;

            switch (activeHandle) {
                case HANDLE_TOP_LEFT: {
                    dragPreviewRect.x = mouseWorld.x;
                    dragPreviewRect.y = mouseWorld.y;
                    dragPreviewRect.width = r.x + r.width - mouseWorld.x;
                    dragPreviewRect.height = r.y + r.height - mouseWorld.y;
                } break;
                case HANDLE_TOP: {
                    dragPreviewRect.y = mouseWorld.y;
                    dragPreviewRect.height = r.y + r.height - mouseWorld.y;
                } break;
                case HANDLE_TOP_RIGHT: {
                    dragPreviewRect.y = mouseWorld.y;
                    dragPreviewRect.width = mouseWorld.x - r.x;
                    dragPreviewRect.height = r.y + r.height - mouseWorld.y;
                } break;
                case HANDLE_RIGHT: {
                    dragPreviewRect.width = mouseWorld.x - r.x;
                } break;
                case HANDLE_BOTTOM_RIGHT: {
                    dragPreviewRect.width = mouseWorld.x - r.x;
                    dragPreviewRect.height = mouseWorld.y - r.y;
                } break;
                case HANDLE_BOTTOM: {
                    dragPreviewRect.height = mouseWorld.y - r.y;
                } break;
                case HANDLE_BOTTOM_LEFT: {
                    dragPreviewRect.x = mouseWorld.x;
                    dragPreviewRect.width = r.x + r.width - mouseWorld.x;
                    dragPreviewRect.height = mouseWorld.y - r.y;
                } break;
                case HANDLE_LEFT: {
                    dragPreviewRect.x = mouseWorld.x;
                    dragPreviewRect.width = r.x + r.width - mouseWorld.x;
                } break;
                default: break;
            }
        } else if (editorDragMode == DRAG_MULTI_SELECT) {
            multiSelectRect.x = min(dragStartWorld.x, mouseWorld.x);
            multiSelectRect.y = min(dragStartWorld.y, mouseWorld.y);
            multiSelectRect.width = fabsf(mouseWorld.x - dragStartWorld.x);
            multiSelectRect.height = fabsf(mouseWorld.y - dragStartWorld.y);
        }
    }

    // ── Release ───────────────────────────────────────────────────────────
    if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
        if (editorDragMode == DRAG_PLACING_NEW) {
            if (dragPreviewRect.width > 20.0f && dragPreviewRect.height > 15.0f) {
                LobbyTerrain_St newTerrain = createDefaultTerrain(currentPaletteType, (Vector2) {dragPreviewRect.x, dragPreviewRect.y});
                newTerrain.rect.width = dragPreviewRect.width;
                newTerrain.rect.height = dragPreviewRect.height;
                da_append(&terrains, newTerrain);
            }
        } else if (editorDragMode == DRAG_MOVING && game->selectedTerrainIndex >= 0) {
            Vector2 offset = {dragPreviewRect.x - terrains.items[game->selectedTerrainIndex].rect.x,
                              dragPreviewRect.y - terrains.items[game->selectedTerrainIndex].rect.y};
            moveSelectedByOffset(offset);
        } else if (editorDragMode == DRAG_RESIZING && game->selectedTerrainIndex >= 0) {
            LobbyTerrain_St* selected = &terrains.items[game->selectedTerrainIndex];
            dragPreviewRect.width = max(dragPreviewRect.width, 20.0f);
            dragPreviewRect.height = max(dragPreviewRect.height, 15.0f);
            selected->rect = dragPreviewRect;
        } else if (editorDragMode == DRAG_MULTI_SELECT) {
            da_clear(&selectedIndices);
            for (size_t i = 0; i < terrains.count; ++i) {
                if (terrainsOverlap(&terrains.items[i], &(LobbyTerrain_St) {.rect = multiSelectRect})) {
                    da_append(&selectedIndices, (s32)i);
                }
            }
            if (selectedIndices.count > 0) {
                game->selectedTerrainIndex = da_last(&selectedIndices);
            } else {
                game->selectedTerrainIndex = -1;
            }
        }

        editorDragMode = DRAG_NONE;
        activeHandle = HANDLE_NONE;
        dragPreviewRect = (Rectangle) {0};
        multiSelectRect = (Rectangle) {0};
    }

    // ── Keyboard shortcuts ────────────────────────────────────────────────
    if (IsKeyPressed(KEY_DELETE)) {
        deleteSelected(game);
    }

    if ((IsKeyDown(KEY_LEFT_CONTROL) || IsKeyDown(KEY_RIGHT_CONTROL)) && IsKeyPressed(KEY_C)) {
        da_clear(&clipboard);
        for (size_t i = 0; i < selectedIndices.count; ++i) {
            s32 idx = selectedIndices.items[i];
            if (idx >= 0 && (size_t)idx < terrains.count) {
                da_append(&clipboard, terrains.items[idx]);
            }
        }
        clipboardHasData = (clipboard.count > 0);
    }

    if ((IsKeyDown(KEY_LEFT_CONTROL) || IsKeyDown(KEY_RIGHT_CONTROL)) && IsKeyPressed(KEY_V) && clipboardHasData) {
        Vector2 pastePos = mouseWorld;

        Rectangle groupBox = computeClipboardGroupBox();
        Vector2 anchorOffset = getPasteAnchorOffset(groupBox, pasteAnchorIndex);

        for (size_t i = 0; i < clipboard.count; ++i) {
            LobbyTerrain_St copy = clipboard.items[i];
            copy.rect.x = pastePos.x + (copy.rect.x - groupBox.x) - anchorOffset.x;
            copy.rect.y = pastePos.y + (copy.rect.y - groupBox.y) - anchorOffset.y;
            da_append(&terrains, copy);
        }
    }

    if ((IsKeyDown(KEY_LEFT_CONTROL) || IsKeyDown(KEY_RIGHT_CONTROL)) && IsKeyPressed(KEY_D)) {
        if (selectedIndices.count > 0) {
            Vector2 offset = duplicationOffsetEnabled ? (Vector2) {30.0f, 30.0f} : (Vector2) {0.0f, 0.0f};
            for (size_t i = 0; i < selectedIndices.count; ++i) {
                s32 idx = selectedIndices.items[i];
                if (idx >= 0 && (size_t)idx < terrains.count) {
                    LobbyTerrain_St copy = terrains.items[idx];
                    copy.rect.x += offset.x;
                    copy.rect.y += offset.y;
                    da_append(&terrains, copy);
                }
            }
        }
    }

    if (IsKeyPressed(KEY_O)) {
        duplicationOffsetEnabled = !duplicationOffsetEnabled;
    }

    if (IsKeyPressed(KEY_B)) {
        showSelectionBox = !showSelectionBox;
    }

    if (IsKeyPressed(KEY_H)) {
        showHelpPanel = !showHelpPanel;
    }

    // ── Panel scrolling (mouse wheel over panels takes priority over camera) ──
    f32 wheel = GetMouseWheelMove();
    if (wheel != 0.0f) {
        s32 screenW = systemSettings.video.width;
        s32 screenH = systemSettings.video.height;
        f32 topPanelHeight = 50.0f;
        f32 bottomPanelHeight = 30.0f;
        f32 leftPanelWidth = 220.0f;
        f32 rightPanelWidth = 260.0f;

        bool panelScrolled = false;

        // Left palette
        if (game->showLeftPalette) {
            Rectangle paletteArea = {0.0f, topPanelHeight, leftPanelWidth, (f32)screenH - topPanelHeight - bottomPanelHeight};
            if (CheckCollisionPointRec(mouseScreen, paletteArea)) {
                paletteScrollY -= wheel * 60.0f;
                f32 contentHeight = (f32)__terrainTypeCount * 55.0f;
                f32 visibleHeight = paletteArea.height - 60.0f; // title space
                f32 minScroll = (contentHeight > visibleHeight) ? visibleHeight - contentHeight : 0.0f;
                paletteScrollY = clamp(paletteScrollY, minScroll, 0.0f);
                panelScrolled = true;
            }
        }

        // Help panel
        if (showHelpPanel) {
            const f32 panelW = 520.0f;
            const f32 panelH = 380.0f;
            const f32 panelX = ((f32)screenW - panelW) / 2.0f;
            const f32 panelY = ((f32)screenH - panelH) / 2.0f;
            Rectangle helpArea = {panelX, panelY, panelW, panelH};
            if (CheckCollisionPointRec(mouseScreen, helpArea)) {
                helpScrollY -= wheel * 45.0f;
                f32 contentHeight = 19.0f * 22.0f + 60.0f;
                f32 visibleHeight = panelH - 110.0f;
                f32 minScroll = (contentHeight > visibleHeight) ? visibleHeight - contentHeight : 0.0f;
                helpScrollY = clamp(helpScrollY, minScroll, 0.0f);
                panelScrolled = true;
            }
        }

        // Properties panel (prepared for future content)
        if (game->showPropertiesPanel) {
            Rectangle propArea = {(f32)screenW - rightPanelWidth, topPanelHeight, rightPanelWidth, (f32)screenH - topPanelHeight - bottomPanelHeight};
            if (CheckCollisionPointRec(mouseScreen, propArea)) {
                propertiesScrollY -= wheel * 40.0f;
                propertiesScrollY = clamp(propertiesScrollY, -800.0f, 0.0f); // generous future-proof limit
                panelScrolled = true;
            }
        }

        if (!panelScrolled) {
            // Original camera wheel behaviour
            if (IsKeyDown(KEY_LEFT_CONTROL) || IsKeyDown(KEY_RIGHT_CONTROL)) {
                panMultiplier = clamp(panMultiplier + wheel * 0.1f, 0.0f, 5.0f);
            } else {
                Vector2 mouseWorldBefore = getMouseWorld(game);
                game->cam.zoom += wheel * 0.1f;
                if (game->cam.zoom < 0.2f) game->cam.zoom = 0.2f;
                if (game->cam.zoom > 5.0f) game->cam.zoom = 5.0f;
                Vector2 mouseWorldAfter = getMouseWorld(game);
                game->cam.target.x += mouseWorldBefore.x - mouseWorldAfter.x;
                game->cam.target.y += mouseWorldBefore.y - mouseWorldAfter.y;
            }
        }
    }

    // Camera panning and keys
    Vector2 pan = {0.0f, 0.0f};
    f32 panSpeed = panMultiplier * 600.0f * dt;

    if (IsMouseButtonDown(MOUSE_BUTTON_MIDDLE)) {
        Vector2 delta = GetMouseDelta();
        pan.x = -delta.x / game->cam.zoom;
        pan.y = -delta.y / game->cam.zoom;
    }

    if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT)) pan.x -= panSpeed;
    if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT)) pan.x += panSpeed;
    if (IsKeyDown(KEY_W) || IsKeyDown(KEY_UP)) pan.y -= panSpeed;
    if (IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN)) pan.y += panSpeed;

    game->cam.target.x += pan.x;
    game->cam.target.y += pan.y;

    if (IsKeyPressed(KEY_R)) {
        game->cam.target = (Vector2) {0.0f, 0.0f};
        game->cam.zoom = 1.0f;
    }

    if (IsKeyPressed(KEY_T)) {
        game->cam.target = game->player.position;
    }

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

    Vector2 mouseScreen = GetMousePosition();

    BeginMode2D(game->cam); {
        // Live drag preview
        if (editorDragMode != DRAG_NONE && (dragPreviewRect.width > 0.0f || dragPreviewRect.height > 0.0f)) {
            DrawRectangleRec(dragPreviewRect, Fade(LIME, 0.4f));
            DrawRectangleLinesEx(dragPreviewRect, 3.0f, LIME);
        }

        // Multi-select rectangle
        if (editorDragMode == DRAG_MULTI_SELECT && (multiSelectRect.width > 0.0f || multiSelectRect.height > 0.0f)) {
            DrawRectangleRec(multiSelectRect, Fade(SKYBLUE, 0.3f));
            DrawRectangleLinesEx(multiSelectRect, 2.0f, SKYBLUE);
        }

        // Group bounding box
        if (showSelectionBox && selectedIndices.count > 1) {
            Rectangle groupBox = computeSelectedGroupBox();
            if (groupBox.width > 0.0f || groupBox.height > 0.0f) {
                DrawRectangleRec(groupBox, Fade(ORANGE, 0.15f));
                DrawRectangleLinesEx(groupBox, 4.0f, ORANGE);
            }
        }

        // Highlight all selected terrains
        for (size_t i = 0; i < selectedIndices.count; ++i) {
            s32 idx = selectedIndices.items[i];
            if (idx >= 0 && (size_t)idx < terrains.count) {
                const LobbyTerrain_St* t = &terrains.items[idx];
                DrawRectangleRec(t->rect, Fade(YELLOW, 0.25f));
                DrawRectangleLinesEx(t->rect, 3.0f, YELLOW);
            }
        }

        // Primary selection + resize handles (single selection only)
        if (game->selectedTerrainIndex >= 0 && selectedIndices.count <= 1 && (size_t)game->selectedTerrainIndex < terrains.count) {
            const LobbyTerrain_St* selected = &terrains.items[game->selectedTerrainIndex];

            DrawTextEx(lobby_fonts[FONT18],
                       TextFormat("Selected Type %d", selected->type),
                       (Vector2) {selected->rect.x, selected->rect.y - 25.0f},
                       18.0f, 0.0f, YELLOW);

            if (editorDragMode != DRAG_RESIZING) {
                const f32 hs = 8.0f / game->cam.zoom;

                Rectangle r = selected->rect;
                Rectangle handles[8];
                fillResizeHandles(handles, r, hs);

                for (s32 i = 0; i < 8; ++i) {
                    DrawRectangleRec(handles[i], WHITE);
                    DrawRectangleLinesEx(handles[i], 2.0f, BLACK);
                }
            }
        }

        DrawCircleV((Vector2) {0.0f, 0.0f}, 12.0f / game->cam.zoom, Fade(GREEN, 0.6f));
        DrawCircleLinesV((Vector2) {0.0f, 0.0f}, 12.0f / game->cam.zoom, GREEN);
    } EndMode2D();

    // ── Screen-space UI (outside camera) ─────────────────────────────────────

    s32 screenW = systemSettings.video.width;
    s32 screenH = systemSettings.video.height;

    f32 topPanelHeight = 50.0f;
    f32 bottomPanelHeight = 30.0f;
    f32 leftPanelWidth = 220.0f;
    f32 rightPanelWidth = 260.0f;

    // Top toolbar
    DrawRectangle(0, 0, screenW, topPanelHeight, Fade(BLACK, 0.75f));
    DrawTextEx(lobby_fonts[FONT20], "EDITOR MODE", (Vector2) {20.0f, 12.0f}, 20.0f, 0.0f, YELLOW);

    DrawRectangle(300, 8, 70, 34, Fade(DARKGRAY, 0.9f));
    DrawTextEx(lobby_fonts[FONT18], "Save", (Vector2) {315.0f, 14.0f}, 18.0f, 0.0f, WHITE);

    DrawRectangle(390, 8, 80, 34, Fade(DARKGRAY, 0.9f));
    DrawTextEx(lobby_fonts[FONT18], "Load", (Vector2) {405.0f, 14.0f}, 18.0f, 0.0f, WHITE);

    // Left palette - scrollable
    if (game->showLeftPalette) {
        DrawRectangle(0, topPanelHeight, leftPanelWidth, screenH - (topPanelHeight + bottomPanelHeight), Fade(BLACK, 0.8f));
        DrawTextEx(lobby_fonts[FONT20], "Terrain Types", (Vector2) {20.0f, 65.0f}, 20.0f, 0.0f, WHITE);

        f32 slotVisibleY = topPanelHeight + 60.0f;
        f32 slotVisibleH = (f32)screenH - topPanelHeight - bottomPanelHeight - 60.0f;

        BeginScissorMode(0, (int)slotVisibleY, (int)leftPanelWidth, (int)slotVisibleH);
        for (u32 i = 0; i < __terrainTypeCount; ++i) {
            f32 slotY = 110.0f + (f32)i * 55.0f + paletteScrollY;
            Color slotColor = (i == currentPaletteType) ? Fade(LIME, 0.7f) : Fade(GRAY, 0.6f);
            DrawRectangle(20, slotY, 180, 45, slotColor);
            DrawTextEx(lobby_fonts[FONT18], terrainTypeNames[i], (Vector2) {35.0f, slotY + 12.0f}, 18.0f, 0.0f, LIGHTGRAY);
        }
        EndScissorMode();
    }

    // Right properties panel - scrollable (prepared for future use)
    if (game->showPropertiesPanel) {
        DrawRectangle(screenW - rightPanelWidth, topPanelHeight, rightPanelWidth, screenH - (topPanelHeight + bottomPanelHeight), Fade(BLACK, 0.8f));
        DrawTextEx(lobby_fonts[FONT20], "Properties", (Vector2) {(f32)screenW - rightPanelWidth + 20.0f, 65.0f}, 20.0f, 0.0f, WHITE);

        f32 propVisibleY = topPanelHeight + 90.0f;
        f32 propVisibleH = (f32)screenH - topPanelHeight - bottomPanelHeight - 100.0f;

        BeginScissorMode(screenW - (int)rightPanelWidth, (int)propVisibleY, (int)rightPanelWidth, (int)propVisibleH);
        // Future properties content would be drawn here with + propertiesScrollY offset
        DrawTextEx(lobby_fonts[FONT18], "No selection (scroll ready)", (Vector2) {(f32)screenW - rightPanelWidth + 20.0f, propVisibleY - propertiesScrollY}, 18.0f, 0.0f, GRAY);
        EndScissorMode();
    }

    // Paste anchor diagram
    if (clipboardHasData && editorDragMode == DRAG_NONE) {
        f32 diagramX = (f32)screenW - 100.0f;
        if (game->showPropertiesPanel) {
            diagramX -= rightPanelWidth;
        }

        Vector2 base = (Vector2) {diagramX, 80.0f};
        f32 s = 12.0f;
        Rectangle anchors[9] = {
            {base.x,          base.y,          s, s},
            {base.x + s*2.5f, base.y,          s, s},
            {base.x + s*5.0f, base.y,          s, s},
            {base.x,          base.y + s*1.5f, s, s},
            {base.x + s*2.5f, base.y + s*1.5f, s, s},
            {base.x + s*5.0f, base.y + s*1.5f, s, s},
            {base.x,          base.y + s*3.0f, s, s},
            {base.x + s*2.5f, base.y + s*3.0f, s, s},
            {base.x + s*5.0f, base.y + s*3.0f, s, s}
        };

        DrawLineV((Vector2) {anchors[0].x + s/2, anchors[0].y + s/2}, (Vector2) {anchors[2].x + s/2, anchors[2].y + s/2}, GRAY);
        DrawLineV((Vector2) {anchors[0].x + s/2, anchors[0].y + s/2}, (Vector2) {anchors[6].x + s/2, anchors[6].y + s/2}, GRAY);
        DrawLineV((Vector2) {anchors[2].x + s/2, anchors[2].y + s/2}, (Vector2) {anchors[8].x + s/2, anchors[8].y + s/2}, GRAY);
        DrawLineV((Vector2) {anchors[6].x + s/2, anchors[6].y + s/2}, (Vector2) {anchors[8].x + s/2, anchors[8].y + s/2}, GRAY);

        for (s32 i = 0; i < 9; ++i) {
            Color c = (i == pasteAnchorIndex) ? YELLOW : GRAY;
            DrawCircleV((Vector2) {anchors[i].x + s/2, anchors[i].y + s/2}, s/2, c);
            DrawCircleLinesV((Vector2) {anchors[i].x + s/2, anchors[i].y + s/2}, s/2, BLACK);
        }
    }

    // Hover info
    Vector2 mouseWorld = getMouseWorld(game);
    bool hoveringSelected = false;
    const char* hoverTypeName = NULL;
    s32 hoverIndex = -1;
    for (size_t i = 0; i < selectedIndices.count; ++i) {
        s32 idx = selectedIndices.items[i];
        if (idx >= 0 && (size_t)idx < terrains.count) {
            const LobbyTerrain_St* t = &terrains.items[idx];
            if (CheckCollisionPointRec(mouseWorld, t->rect)) {
                hoveringSelected = true;
                hoverTypeName = terrainTypeNames[t->type];
                hoverIndex = idx;
                break;
            }
        }
    }
    if (hoveringSelected && hoverTypeName) {
        Vector2 textPos = {mouseScreen.x + 25.0f, mouseScreen.y - 35.0f};
        const char* label = TextFormat("Type: %s (%d)", hoverTypeName, hoverIndex);

        for (int ox = -1; ox <= 1; ++ox) {
            for (int oy = -1; oy <= 1; ++oy) {
                if (ox == 0 && oy == 0) continue;
                DrawTextEx(lobby_fonts[FONT18], label,
                           (Vector2) {textPos.x + (f32)ox * 1.5f, textPos.y + (f32)oy * 1.5f},
                           18.0f, 0.0f, BLACK);
            }
        }
        DrawTextEx(lobby_fonts[FONT18], label, textPos, 18.0f, 0.0f, WHITE);
    }

    // Help panel - now scrollable
    if (showHelpPanel) {
        const f32 panelW = 520.0f;
        const f32 panelH = 380.0f;
        const f32 panelX = (screenW - panelW) / 2.0f;
        const f32 panelY = (screenH - panelH) / 2.0f;

        DrawRectangle(panelX - 10, panelY - 10, panelW + 20, panelH + 20, Fade(BLACK, 0.85f));
        DrawRectangle(panelX, panelY, panelW, panelH, Fade(DARKGRAY, 0.95f));
        DrawRectangleLinesEx((Rectangle){panelX, panelY, panelW, panelH}, 4.0f, YELLOW);

        DrawTextEx(lobby_fonts[FONT24], "EDITOR HELP - KEYBINDS", (Vector2){panelX + 20, panelY + 15}, 24, 0, YELLOW);

        const char* lines[] = {
            "F1                Toggle editor mode",
            "Left Click        Select / Move / Resize / Place new terrain",
            "Shift + Click     Toggle terrain in multi-selection",
            "Shift + Drag      Multi-select box (on empty space)",
            "Delete            Delete all selected terrains",
            "Ctrl + C          Copy selected terrains",
            "Ctrl + V          Paste at mouse (anchor diagram shown)",
            "Ctrl + D          Duplicate selected (offset toggleable)",
            "O                 Toggle duplication offset",
            "B                 Toggle group bounding box",
            "H                 Toggle this help panel",
            "Q                 Toggle left palette",
            "P                 Toggle properties panel",
            "R                 Reset camera to (0,0)",
            "T                 Reset camera to player",
            "Mouse Wheel       Zoom (centered on cursor)",
            "Ctrl + Wheel      Adjust pan speed",
            "Middle Mouse Drag Pan camera",
            "A / D / W / S     Pan camera (arrow keys also work)"
        };

        f32 contentStartY = panelY + 55.0f;
        f32 visibleContentH = panelH - 110.0f;

        BeginScissorMode((int)panelX, (int)contentStartY, (int)panelW, (int)visibleContentH);
        for (u32 i = 0; i < ARRAY_LEN(lines); ++i) {
            f32 y = contentStartY + (f32)i * 22.0f + helpScrollY;
            DrawTextEx(lobby_fonts[FONT18], lines[i], (Vector2){panelX + 30, y}, 18, 0, LIGHTGRAY);
        }
        EndScissorMode();

        DrawTextEx(lobby_fonts[FONT18], "Click anywhere or press H to close", (Vector2){panelX + 30, panelY + panelH - 30}, 18, 0, ORANGE);
    }

    // Bottom status bar
    DrawRectangle(0, screenH - bottomPanelHeight, screenW, bottomPanelHeight, Fade(BLACK, 0.75f));

    Vector2 mouseWorldStatus = getMouseWorld(game);
    const char* dragStr = (editorDragMode == DRAG_NONE) ? "idle" :
                          (editorDragMode == DRAG_PLACING_NEW ? "placing" :
                          (editorDragMode == DRAG_MOVING ? "moving" :
                          (editorDragMode == DRAG_RESIZING ? "resizing" : "multi-select")));

    DrawTextEx(lobby_fonts[FONT18],
               TextFormat("Mouse: %.1f, %.1f   Zoom: %.2f   Terrains: %u   Selected: %zu   Drag: %s   Box: %s",
                          mouseWorldStatus.x, mouseWorldStatus.y, game->cam.zoom, terrains.count, selectedIndices.count, dragStr,
                          showSelectionBox ? "ON" : "OFF"),
               (Vector2) {20.0f, (f32)screenH - 25.0f},
               18.0f, 0.0f, LIGHTGRAY);
}