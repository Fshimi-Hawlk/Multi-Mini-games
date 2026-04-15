/**
    @file editor/input.c
    @author Fshimi-Hawlk
    @date 2026-03-27
    @date 2026-04-13
    @brief Input handling, drag logic, multi-select and keyboard shortcuts for the level editor.

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
            - Hover info for selected terrains
            - Group bounding box robustness on shift-click multi-select
            - Scrolling frames for help panel, terrain palette and properties panel
            - Full extraction and deduplication of input logic
            - Fixed inverted scrolling
            - Help panel now closes on any click (matches the help text)
            - Fixed multi-select drag-to-move: now correctly moves the whole group
              when clicking/dragging any selected terrain while multiple are selected.
            - Integrated the new reusable widget system for the properties panel
            - Proper calls to handlePropertiesMultiSelectClick and updatePropertiesPanel

    @see `editor/editor.h`
    @see `editor/types.h`
    @see `editor/utils.h`
    @see `editor/properties.h`
*/

#include "editor/types.h"
#include "editor/editor.h"
#include "editor/utils.h"
#include "editor/properties.h"
#include "editor/io.h"
#include "editor/codegen.h"

#include "utils/globals.h"

#include "sharedWidgets/scrollFrame.h"
#include "sharedWidgets/button.h"

#include "sharedUtils/mathUtils.h"

// ── Public API ──────────────────────────────────────────────────────────────

void updateEditor(LobbyGame_St* const game, f32 dt) {
    if (!game->editorMode) return;

    if (highlightTimer > 0.0f) {
        highlightTimer -= dt;
        if (highlightTimer < 0.0f) highlightTimer = 0.0f;
    }

    Vector2 mouseScreen = GetMousePosition();
    Vector2 mouseWorld = getMouseWorld(game);

    if (IsKeyPressed(KEY_F1)) {
        extern void switchMinigame(u8 gameId);
        switchMinigame(MINI_GAME_ID_LOBBY);
    }

    if (textButtonUpdate(&btnLoad, mouseScreen)) {
        char* selected = editorShowOpenDialog(NULL);
        if (selected) {
            editorLoadLevel(selected);
            free(selected);
        }
    }

    if (textButtonUpdate(&btnSave, mouseScreen)) {
        char* selected = editorShowSaveDialog("LevelName", ".dat");
        if (selected) {
            editorSaveLevel(selected);
            free(selected);
        }
    }

    if (textButtonUpdate(&btnGene, mouseScreen)) {
        char* selected = editorShowSaveDialog("generated-levelName", ".c");
        if (selected) {
            editorGenerateCode(game, selected);
            free(selected);
        }
    }

    // Palette selection (scroll-aware)
    if (game->showLeftPalette && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        if (mouseScreen.x < 220.0f) {
            f32 slotBaseY = 110.0f;
            f32 logicalY = mouseScreen.y - slotBaseY + paletteScroll.scroll.y;
            s32 slot = (s32)(logicalY / 55.0f);
            if (slot >= 0 && slot < __terrainKindCount) {
                currentPaletteKind = (TerrainKind_Et)slot;
            }
        }
    }

    // ── Properties panel update (widgets are updated here) ─────────────────
    if (game->showPropertiesPanel) {
        updatePropertiesPanel(game);
    }

    // ── Mouse button pressed ───────────────────────────────────────────────
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        bool clickedOnUI = (game->showLeftPalette && mouseScreen.x < 220.0f) ||
                           (game->showPropertiesPanel && mouseScreen.x > systemSettings.video.width - 260.0f);

        if (clickedOnUI) return;

        if (portalTargetPickMode) {
            s32 hit = findTerrainAtPoint(mouseWorld);
            bool isTwoWay = cbTwoWay.checked;

            if (hit != -1 && terrains.items[hit].kind == TERRAIN_KIND_PORTAL) {
                // Clicked another portal
                terrains.items[portalBeingConfigured].portalTargetPosition = 
                    (Vector2){terrains.items[hit].rect.x + terrains.items[hit].rect.width * 0.5f,
                              terrains.items[hit].rect.y + terrains.items[hit].rect.height * 0.5f};

                if (isTwoWay) {
                    terrains.items[hit].portalTargetPosition = 
                        (Vector2){terrains.items[portalBeingConfigured].rect.x + 
                                  terrains.items[portalBeingConfigured].rect.width * 0.5f,
                                  terrains.items[portalBeingConfigured].rect.y + 
                                  terrains.items[portalBeingConfigured].rect.height * 0.5f};
                }

                // Visual feedback
                highlightPortalIndex = hit;
                highlightOtherPortalIndex = isTwoWay ? portalBeingConfigured : -1;
                highlightTimer = 1.5f;

            } else {
                // Empty space click
                terrains.items[portalBeingConfigured].portalTargetPosition = mouseWorld;

                // Feedback on the portal we are configuring
                highlightPortalIndex = portalBeingConfigured;
                highlightOtherPortalIndex = -1;
                highlightTimer = 1.5f;
            }

            portalTargetPickMode = false;
            portalBeingConfigured = -1;
            refreshPropertyBuffers(game);
            return;
        }

        bool shiftDown = IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT);

        if (shiftDown) {
            s32 hit = findTerrainAtPoint(mouseWorld);
            if (hit != -1) {
                bool already = false;
                for (size_t i = 0; i < selectedIndices.count; ++i) {
                    if (selectedIndices.items[i] == hit) {
                        da_remove_unordered(&selectedIndices, i);
                        already = true;
                        break;
                    }
                }

                if (!already) da_append(&selectedIndices, hit);
                game->selectedTerrainIndex = (selectedIndices.count > 0) ? da_last(&selectedIndices) : -1;

                refreshPropertyBuffers(game);
            } else {
                editorDragMode = DRAG_MULTI_SELECT;
                dragStartWorld = mouseWorld;
                multiSelectRect = (Rectangle) {mouseWorld.x, mouseWorld.y, 0.0f, 0.0f};
            }

            return;
        }

        // ── Normal left click (no Shift) ─────────────────────────────────────
        s32 hitZone = findZoneAtPoint(mouseWorld);
        if (hitZone != -1) {
            selectedZoneIndex = hitZone;
            editorDragMode = DRAG_MOVING_ZONE;
            dragStartWorld = mouseWorld;

            // deselect terrains
            da_clear(&selectedIndices);
            game->selectedTerrainIndex = -1;
            refreshPropertyBuffers(game);
            return;
        }

        s32 hit = findTerrainAtPoint(mouseWorld);

        if (selectedIndices.count > 1) {
            Rectangle groupBox = computeSelectedGroupBox();
            if (groupBox.width > 0.0f && groupBox.height > 0.0f && CheckCollisionPointRec(mouseWorld, groupBox)) {
                editorDragMode = DRAG_MOVING;
                dragStartWorld = mouseWorld;
                dragPreviewRect = groupBox;
                
                refreshPropertyBuffers(game);
                return;
            }
        }

        if (game->selectedTerrainIndex >= 0) {
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
                    
                refreshPropertyBuffers(game);
                    return;
                }
            }

            // Clicked on the already selected terrain
            if (CheckCollisionPointRec(mouseWorld, selected->rect)) {
                editorDragMode = DRAG_MOVING;
                dragStartWorld = mouseWorld;
                dragPreviewRect = selected->rect;
                
                refreshPropertyBuffers(game);
            } else if (hit != -1) {
                // Clicked on a different terrain
                if (immediateSelectOnClick) {
                    game->selectedTerrainIndex = hit;
                    da_clear(&selectedIndices);
                    da_append(&selectedIndices, hit);

                    focusedTerrainIndex = hit;                    // enter single-terrain edit mode
                    refreshPropertyBuffers(game);                 // sync the widgets

                    if (immediateDragAfterSelect) {
                        editorDragMode = DRAG_MOVING;
                        dragStartWorld = mouseWorld;
                        dragPreviewRect = terrains.items[hit].rect;
                    } else {
                        editorDragMode = DRAG_NONE;
                    }
                } else {
                    editorDragMode = DRAG_NONE;
                }
            }  else {
                // Clicked on empty space -> deselect
                game->selectedTerrainIndex = -1;
                focusedTerrainIndex = -1;
                da_clear(&selectedIndices);
                editorDragMode = DRAG_NONE;
                activeHandle = HANDLE_NONE;
                
                refreshPropertyBuffers(game);
            }
        } else {
            // Nothing was selected
            if (hit == -1) {
                editorDragMode = DRAG_PLACING_NEW;
                dragStartWorld = mouseWorld;
                dragPreviewRect = (Rectangle){mouseWorld.x, mouseWorld.y, 0.0f, 0.0f};
            } else {
                // First click on a terrain
                game->selectedTerrainIndex = hit;
                da_clear(&selectedIndices);
                da_append(&selectedIndices, hit);

                if (immediateDragAfterSelect) {
                    editorDragMode = DRAG_MOVING;
                    dragStartWorld = mouseWorld;
                    dragPreviewRect = terrains.items[hit].rect;
                } else {
                    editorDragMode = DRAG_NONE;
                }
                activeHandle = HANDLE_NONE;
                
                refreshPropertyBuffers(game);
            }
        }
    }

    // ── Live drag ───────────────────────────────────────────────────────────
    if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
        Vector2 offset = {mouseWorld.x - dragStartWorld.x, mouseWorld.y - dragStartWorld.y};
        
        if (editorDragMode == DRAG_PLACING_NEW) {
            dragPreviewRect.x = min(dragStartWorld.x, mouseWorld.x);
            dragPreviewRect.y = min(dragStartWorld.y, mouseWorld.y);
            dragPreviewRect.width = fabsf(mouseWorld.x - dragStartWorld.x);
            dragPreviewRect.height = fabsf(mouseWorld.y - dragStartWorld.y);
        } else if (editorDragMode == DRAG_MOVING_ZONE && selectedZoneIndex != -1) {
            dragPreviewRect = gameZones[selectedZoneIndex].hitbox;
            dragPreviewRect.x += offset.x;
            dragPreviewRect.y += offset.y;
        } else if (editorDragMode == DRAG_MOVING) {
            if (selectedIndices.count > 1) {
                dragPreviewRect.x = computeSelectedGroupBox().x + offset.x;
                dragPreviewRect.y = computeSelectedGroupBox().y + offset.y;
            } else if (game->selectedTerrainIndex >= 0) {
                dragPreviewRect.x = terrains.items[game->selectedTerrainIndex].rect.x + offset.x;
                dragPreviewRect.y = terrains.items[game->selectedTerrainIndex].rect.y + offset.y;
            }
        } else if (editorDragMode == DRAG_RESIZING && activeHandle != HANDLE_NONE && game->selectedTerrainIndex >= 0) {
            const LobbyTerrain_St* original = &terrains.items[game->selectedTerrainIndex];
            Rectangle r = original->rect;
            switch (activeHandle) {
                case HANDLE_TOP_LEFT:    dragPreviewRect.x = mouseWorld.x; dragPreviewRect.y = mouseWorld.y; dragPreviewRect.width = r.x + r.width - mouseWorld.x; dragPreviewRect.height = r.y + r.height - mouseWorld.y; break;
                case HANDLE_TOP:         dragPreviewRect.y = mouseWorld.y; dragPreviewRect.height = r.y + r.height - mouseWorld.y; break;
                case HANDLE_TOP_RIGHT:   dragPreviewRect.y = mouseWorld.y; dragPreviewRect.width = mouseWorld.x - r.x; dragPreviewRect.height = r.y + r.height - mouseWorld.y; break;
                case HANDLE_RIGHT:       dragPreviewRect.width = mouseWorld.x - r.x; break;
                case HANDLE_BOTTOM_RIGHT:dragPreviewRect.width = mouseWorld.x - r.x; dragPreviewRect.height = mouseWorld.y - r.y; break;
                case HANDLE_BOTTOM:      dragPreviewRect.height = mouseWorld.y - r.y; break;
                case HANDLE_BOTTOM_LEFT: dragPreviewRect.x = mouseWorld.x; dragPreviewRect.width = r.x + r.width - mouseWorld.x; dragPreviewRect.height = mouseWorld.y - r.y; break;
                case HANDLE_LEFT:        dragPreviewRect.x = mouseWorld.x; dragPreviewRect.width = r.x + r.width - mouseWorld.x; break;
                default: break;
            }
        } else if (editorDragMode == DRAG_MULTI_SELECT) {
            multiSelectRect.x = min(dragStartWorld.x, mouseWorld.x);
            multiSelectRect.y = min(dragStartWorld.y, mouseWorld.y);
            multiSelectRect.width = fabsf(mouseWorld.x - dragStartWorld.x);
            multiSelectRect.height = fabsf(mouseWorld.y - dragStartWorld.y);
        }
    }

    // ── Mouse release ───────────────────────────────────────────────────────
    if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
        if (editorDragMode == DRAG_PLACING_NEW) {
            if (dragPreviewRect.width > 20.0f && dragPreviewRect.height > 15.0f) {
                LobbyTerrain_St newTerrain = createDefaultTerrain(currentPaletteKind, (Vector2) {dragPreviewRect.x, dragPreviewRect.y});
                newTerrain.rect.width = dragPreviewRect.width;
                newTerrain.rect.height = dragPreviewRect.height;
                da_append(&terrains, newTerrain);
            }
        } else if (editorDragMode == DRAG_MOVING_ZONE && selectedZoneIndex != -1) {
            Vector2 finalOffset = {mouseWorld.x - dragStartWorld.x, mouseWorld.y - dragStartWorld.y};
            gameZones[selectedZoneIndex].hitbox.x += finalOffset.x;
            gameZones[selectedZoneIndex].hitbox.y += finalOffset.y;
        } else if (editorDragMode == DRAG_MOVING) {
            Vector2 finalOffset = {mouseWorld.x - dragStartWorld.x, mouseWorld.y - dragStartWorld.y};
            moveSelectedByOffset(finalOffset);
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
            game->selectedTerrainIndex = (selectedIndices.count > 0) ? da_last(&selectedIndices) : -1;
        }

        editorDragMode = DRAG_NONE;
        activeHandle = HANDLE_NONE;
        dragPreviewRect = (Rectangle) {0};
        multiSelectRect = (Rectangle) {0};
        refreshPropertyBuffers(game);
    }

    // ── Keyboard shortcuts ─────────────────────────────────────────────────
    if (IsKeyPressed(KEY_DELETE)) {
        if (selectedIndices.count > 0) {
            for (s32 i = (s32)selectedIndices.count - 1; i >= 0; --i) {
                u32 idx = (u32)selectedIndices.items[i];
                if (idx < terrains.count) da_remove_unordered(&terrains, idx);
            }
        } else if (game->selectedTerrainIndex >= 0) {
            da_remove_unordered(&terrains, (u32)game->selectedTerrainIndex);
        }

        game->selectedTerrainIndex = -1;
        da_clear(&selectedIndices);
        editorDragMode = DRAG_NONE;
        activeHandle = HANDLE_NONE;

        refreshPropertyBuffers(game);
    }

    if ((IsKeyDown(KEY_LEFT_CONTROL) || IsKeyDown(KEY_RIGHT_CONTROL)) && IsKeyPressed(KEY_C)) {
        da_clear(&clipboard);
        for (size_t i = 0; i < selectedIndices.count; ++i) {
            s32 idx = selectedIndices.items[i];
            if (idx >= 0 && (size_t)idx < terrains.count) da_append(&clipboard, terrains.items[idx]);
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

    if (IsKeyPressed(KEY_O)) duplicationOffsetEnabled = !duplicationOffsetEnabled;
    if (IsKeyPressed(KEY_B)) showSelectionBox = !showSelectionBox;
    if (IsKeyPressed(KEY_H)) showHelpPanel = !showHelpPanel;
    if (IsKeyPressed(KEY_I)) immediateSelectOnClick   = !immediateSelectOnClick;
    if (IsKeyPressed(KEY_J)) immediateDragAfterSelect = !immediateDragAfterSelect;

    // ── Panel scrolling (wheel over panels) ────────────────────────────────
    f32 wheel = GetMouseWheelMove();
    if (wheel != 0.0f) {
        s32 screenW = systemSettings.video.width;
        s32 screenH = systemSettings.video.height;
        f32 topPanelHeight = 50.0f;
        f32 bottomPanelHeight = 30.0f;
        f32 leftPanelWidth = 220.0f;
        f32 rightPanelWidth = 260.0f;

        bool panelScrolled = false;

        if (game->showLeftPalette) {
            Rectangle paletteArea = {0.0f, topPanelHeight, leftPanelWidth, (f32)screenH - topPanelHeight - bottomPanelHeight};
            if (CheckCollisionPointRec(mouseScreen, paletteArea)) {
                panelScrolled = scrollFrameUpdate(&paletteScroll, paletteArea, mouseScreen);
            }
        }

        if (showHelpPanel) {
            const f32 panelW = 520.0f;
            const f32 panelH = 380.0f;
            const f32 panelX = ((f32)screenW - panelW) / 2.0f;
            const f32 panelY = ((f32)screenH - panelH) / 2.0f;
            Rectangle helpArea = {panelX, panelY, panelW, panelH};

            if (CheckCollisionPointRec(mouseScreen, helpArea)) {
                panelScrolled = scrollFrameUpdate(&helpScroll, helpArea, mouseScreen);
            }
        }

        if (game->showPropertiesPanel) {
            Rectangle propArea = {(f32)screenW - rightPanelWidth, topPanelHeight, rightPanelWidth, (f32)screenH - topPanelHeight - bottomPanelHeight};
            if (CheckCollisionPointRec(mouseScreen, propArea)) {
                panelScrolled = scrollFrameUpdate(&propertiesScroll, propArea, mouseScreen);
            }

            handlePropertiesMultiSelectClick(game, mouseScreen);
        }

        if (!panelScrolled) {
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

    // Camera panning (middle mouse + WASD/arrows)
    Vector2 pan = {0.0f, 0.0f};
    f32 panSpeed = panMultiplier * 600.0f * dt;

    if (!editorAnyTextBoxActive) {
        if (IsMouseButtonDown(MOUSE_BUTTON_MIDDLE)) {
            Vector2 delta = GetMouseDelta();
            pan.x = -delta.x / game->cam.zoom;
            pan.y = -delta.y / game->cam.zoom;
        }
    
        if (IsKeyDown(KEY_A)) pan.x -= panSpeed;
        if (IsKeyDown(KEY_D)) pan.x += panSpeed;
        if (IsKeyDown(KEY_W)) pan.y -= panSpeed;
        if (IsKeyDown(KEY_S)) pan.y += panSpeed;
        
        game->cam.target.x += pan.x;
        game->cam.target.y += pan.y;
    
        if (IsKeyPressed(KEY_R)) {
            game->cam.target = (Vector2) {0.0f, 0.0f};
            game->cam.zoom = 1.0f;
        }
    
        if (IsKeyPressed(KEY_T)) {
            game->cam.target = game->player.position;
        }
    
        if (IsKeyPressed(KEY_Q)) game->showLeftPalette = !game->showLeftPalette;
        if (IsKeyPressed(KEY_P)) game->showPropertiesPanel = !game->showPropertiesPanel;
    }
}