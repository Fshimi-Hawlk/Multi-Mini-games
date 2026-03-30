/**
    @file editor/draw.c
    @author Fshimi-Hawlk
    @date 2026-03-27
    @date 2026-03-27
    @brief All rendering code for the level editor (world-space + screen-space UI).

    Contributors:
        - Fshimi-Hawlk:
            - Original drawing logic
            - Full refactoring, helper usage, deduplication, scrolling panels
            - Drag preview and live movement now use the full selected group instead of only
              the single `selectedTerrainIndex`.

    @see `editor/editor.h`
    @see `editor/utils.h`
*/

#include "editor/types.h"
#include "editor/editor.h"
#include "editor/utils.h"
#include "editor/properties.h"

#include "widgets/scrollFrame.h"
#include "widgets/textBox.h"
#include "widgets/button.h"
#include "widgets/slider.h"
#include "widgets/dropdown.h"
#include "widgets/checkBox.h"

#include "utils/globals.h"
#include "utils/utils.h"

#include "systemSettings.h"

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

void drawEditor(const LobbyGame_St* const game) {
    if (!game->editorMode) return;

    Vector2 mouseScreen = GetMousePosition();

    BeginMode2D(game->cam); {
        // Live drag preview (now works for both single and multi-select)
        if (editorDragMode != DRAG_NONE) {
            if (editorDragMode == DRAG_PLACING_NEW) {
                if (dragPreviewRect.width > 0.0f || dragPreviewRect.height > 0.0f) {
                    DrawRectangleRec(dragPreviewRect, Fade(LIME, 0.35f));
                    DrawRectangleLinesEx(dragPreviewRect, 3.0f, LIME);
                }
            } else if (editorDragMode == DRAG_MOVING) {
                if (selectedIndices.count > 1) {
                    // Group bounding box preview
                    DrawRectangleRec(dragPreviewRect, Fade(LIME, 0.35f));
                    DrawRectangleLinesEx(dragPreviewRect, 3.5f, LIME);

                    // Individual terrain previews (ghosts)
                    for (size_t i = 0; i < selectedIndices.count; ++i) {
                        s32 idx = selectedIndices.items[i];
                        if (idx >= 0 && (size_t)idx < terrains.count) {
                            Rectangle r = terrains.items[idx].rect;
                            r.x += (dragPreviewRect.x - computeSelectedGroupBox().x);  // relative offset
                            r.y += (dragPreviewRect.y - computeSelectedGroupBox().y);
                            DrawRectangleRec(r, Fade(LIME, 0.22f));
                            DrawRectangleLinesEx(r, 2.8f, LIME);
                        }
                    }
                } else if (game->selectedTerrainIndex >= 0) {
                    // Single terrain preview
                    DrawRectangleRec(dragPreviewRect, Fade(LIME, 0.4f));
                    DrawRectangleLinesEx(dragPreviewRect, 3.0f, LIME);
                }
            }
        }

        // Multi-select rectangle
        if (editorDragMode == DRAG_MULTI_SELECT && (multiSelectRect.width > 0.0f || multiSelectRect.height > 0.0f)) {
            DrawRectangleRec(multiSelectRect, Fade(SKYBLUE, 0.3f));
            DrawRectangleLinesEx(multiSelectRect, 2.0f, SKYBLUE);
        }

        // Group bounding box (when not dragging)
        if (showSelectionBox && selectedIndices.count > 1 && editorDragMode != DRAG_MOVING) {
            Rectangle groupBox = computeSelectedGroupBox();
            if (groupBox.width > 0.0f || groupBox.height > 0.0f) {
                DrawRectangleRec(groupBox, Fade(ORANGE, 0.15f));

                if (!propertiesBeingEdited) {
                    DrawRectangleLinesEx(groupBox, 4.0f, ORANGE);
                }
            }
        }

        // Highlight all selected terrains (always show, even during move)
        for (size_t i = 0; i < selectedIndices.count; ++i) {
            s32 idx = selectedIndices.items[i];
            if (idx >= 0 && (size_t)idx < terrains.count) {
                const LobbyTerrain_St* t = &terrains.items[idx];
                DrawRectangleRec(t->rect, Fade(YELLOW, 0.25f));

                if (!propertiesBeingEdited) {
                    DrawRectangleLinesEx(t->rect, 3.0f, YELLOW);
                }
            }
        }

        // Primary selection + resize handles (single selection only)
        if (game->selectedTerrainIndex >= 0 && selectedIndices.count <= 1 && (size_t)game->selectedTerrainIndex < terrains.count) {
            const LobbyTerrain_St* selected = &terrains.items[game->selectedTerrainIndex];

            if (editorDragMode != DRAG_RESIZING && !propertiesBeingEdited) {
                const f32 hs = 8.0f / game->cam.zoom;
                Rectangle r = selected->rect;
                Rectangle handles[8];
                fillResizeHandles(handles, r, hs);

                for (s32 i = 0; i < 8; ++i) {
                    DrawRectangleRec(handles[i], WHITE);
                    DrawRectangleLinesEx(handles[i], 2.0f, BLACK);
                }
            } else {
                // Single terrain preview
                DrawRectangleRec(dragPreviewRect, Fade(LIME, 0.4f));

                if (!propertiesBeingEdited) {
                    DrawRectangleLinesEx(dragPreviewRect, 3.0f, LIME);
                }
            }
        }

        if (propertiesBeingEdited && (focusedTerrainIndex != -1 || selectedIndices.count == 1)) {
            s32 idx = (focusedTerrainIndex != -1) ? focusedTerrainIndex : selectedIndices.items[0];
            Rectangle preview = getEditedTerrainPreviewRect(idx);
            if (preview.width > 0.0f && preview.height > 0.0f) {
                DrawRectangleRec(preview, Fade(LIME, 0.25f));
                DrawRectangleLinesEx(preview, 3.0f, LIME);
            }
        }

        // Portal target pick mode preview
        if (portalTargetPickMode && portalBeingConfigured >= 0) {
            Vector2 mouseWorld = getMouseWorld(game);
            Rectangle current = terrains.items[portalBeingConfigured].rect;
            Vector2 from = (Vector2){current.x + current.width * 0.5f, current.y + current.height * 0.5f};

            // Dashed line
            DrawLineEx(from, mouseWorld, 3.0f, Fade(PURPLE, 0.6f));

            // Ghost target portal
            Rectangle ghost = {mouseWorld.x - 40, mouseWorld.y - 40, 80, 80};
            DrawRectangleRoundedLinesEx(ghost, 1.0f, 0, 3.0f, Fade(PURPLE, 0.4f));
        }

                // Portal target line + highlight when a portal is selected
        if ((focusedTerrainIndex != -1 || selectedIndices.count == 1) && !portalTargetPickMode) {
            s32 idx = (focusedTerrainIndex != -1) ? focusedTerrainIndex : selectedIndices.items[0];
            if (idx >= 0 && (size_t)idx < terrains.count) {
                const LobbyTerrain_St* p = &terrains.items[idx];
                if (p->type == TERRAIN_PORTAL) {
                    Vector2 from = getRectCenterPos(p->rect);
                    Vector2 to   = p->portalTargetPosition;

                    Color lineColor = (p->isTwoWayPortal) ? PURPLE : SKYBLUE;

                    // Dashed line to target
                    DrawLineEx(from, to, 3.0f, Fade(lineColor, 0.7f));

                    // Highlight the target portal if it exists
                    s32 targetIdx = findTerrainAtPoint(to);
                    if (targetIdx != -1 && terrains.items[targetIdx].type == TERRAIN_PORTAL) {
                        drawPortalHighlight(targetIdx, lineColor, &game->cam);
                    }

                    // If two-way, also highlight the source portal
                    if (p->isTwoWayPortal) {
                        drawPortalHighlight(idx, lineColor, &game->cam);
                    }
                }
            }
        }

        // Portal selection highlight feedback
        if (highlightTimer > 0.0f) {
            f32 alpha = highlightTimer;   // 1.0 → 0.0 fade
            Color yellowFade = Fade(YELLOW, alpha);
            Color greenFade = Fade(GREEN, alpha);

            drawPortalHighlight(highlightPortalIndex, yellowFade, &game->cam);
            if (highlightOtherPortalIndex != -1) {
                drawPortalHighlight(highlightOtherPortalIndex, greenFade, &game->cam);
            }
        }

        DrawCircleV((Vector2) {0.0f, 0.0f}, 12.0f / game->cam.zoom, Fade(GREEN, 0.6f));
        DrawCircleLinesV((Vector2) {0.0f, 0.0f}, 12.0f / game->cam.zoom, GREEN);
    } EndMode2D();

    // ── Screen-space UI ─────────────────────────────────────────────────────
    s32 screenW = systemSettings.video.width;
    s32 screenH = systemSettings.video.height;

    f32 topPanelHeight = 50.0f;
    f32 bottomPanelHeight = 30.0f;
    f32 leftPanelWidth = 220.0f;
    f32 rightPanelWidth = 260.0f;

    // Top toolbar
    DrawRectangle(0, 0, screenW, topPanelHeight, Fade(BLACK, 0.75f));
    DrawTextEx(lobby_fonts[FONT20], "EDITOR MODE", (Vector2) {20.0f, 12.0f}, 20.0f, 0.0f, YELLOW);

    textButtonDraw(&btnLoad, lobby_fonts[FONT18], 18);
    textButtonDraw(&btnSave, lobby_fonts[FONT18], 18);
    textButtonDraw(&btnGenerate, lobby_fonts[FONT18], 18);

    // Left palette
    if (game->showLeftPalette) {
        DrawRectangle(0, topPanelHeight, leftPanelWidth,
                  screenH - (topPanelHeight + bottomPanelHeight),
                  Fade(BLACK, 0.8f));

        DrawTextEx(lobby_fonts[FONT20], "Terrain Types",
                (Vector2) {20.0f, 65.0f}, 20.0f, 0.0f, WHITE);

        Rectangle paletteArea = {
            .x = 0.0f,
            .y = 110.0f,
            .width = leftPanelWidth,
            .height = (f32)screenH - topPanelHeight - bottomPanelHeight * 2
        };

        paletteScroll.contentHeight = (f32)__terrainTypeCount * (55.0f + 12.0f);

        scrollFrameBegin(&paletteScroll, paletteArea); {
            for (u32 i = 0; i < __terrainTypeCount; ++i) {
                f32 slotY = 110.0f + (f32)i * 55.0f - paletteScroll.scrollY;
    
                Color slotColor = (i == currentPaletteType) ? Fade(LIME, 0.7f) : Fade(GRAY, 0.6f);
                DrawRectangle(20, slotY, 180, 45, slotColor);
                DrawTextEx(lobby_fonts[FONT18], terrainTypeNames[i],
                        (Vector2) {35.0f, slotY + 12.0f}, 18.0f, 0.0f, LIGHTGRAY);
            }
        } scrollFrameEnd();
    }

    // Right properties panel
    if (game->showPropertiesPanel) {
        DrawRectangle(
            screenW - rightPanelWidth, topPanelHeight,
            rightPanelWidth, screenH - (topPanelHeight + bottomPanelHeight),
            Fade(BLACK, 0.8f)
        );

        DrawTextEx(
            lobby_fonts[FONT20], "Properties",
            (Vector2) {
                .x = (f32)screenW - rightPanelWidth + 20.0f,
                .y = 65.0f
            },
            20.0f, 0.0f, WHITE
        );

        Rectangle propertiesArea = {
            .x = (f32)screenW - rightPanelWidth,
            .y = 90.0f,
            .width = rightPanelWidth,
            .height = (f32)screenH - (topPanelHeight + 2 * bottomPanelHeight)
        };

        propertiesScroll.contentHeight = 1200.0f;   // large enough for all fields + list
        scrollFrameBegin(&propertiesScroll, propertiesArea); { 
            f32 baseY = 90.0f - propertiesScroll.scrollY;

            if (selectedIndices.count == 0) {
                DrawTextEx(lobby_fonts[FONT18], "No selection",
                        (Vector2) {propertiesArea.x + 20.0f, baseY}, 18.0f, 0.0f, GRAY);
            } else if (focusedTerrainIndex != -1 || selectedIndices.count == 1) {
                // ── SINGLE / FOCUSED TERRAIN EDIT MODE ─────────────────────────────
                s32 idx = (focusedTerrainIndex != -1) ? focusedTerrainIndex : selectedIndices.items[0];
                LobbyTerrain_St* t = &terrains.items[idx];

                f32 y = baseY;

                // Position
                DrawTextEx(lobby_fonts[FONT18], "Position", (Vector2) {propertiesArea.x + 20, y}, 18, 0, WHITE); y += 32;
                tbPosX.bounds = (Rectangle) {propertiesArea.x + 20, y, 100, 28};
                tbPosY.bounds = (Rectangle) {propertiesArea.x + 130, y, 100, 28};
                textBoxDraw(&tbPosX, lobby_fonts[FONT18], 18);
                textBoxDraw(&tbPosY, lobby_fonts[FONT18], 18);
                y += 40;

                // Size
                DrawTextEx(lobby_fonts[FONT18], "Size", (Vector2) {propertiesArea.x + 20, y}, 18, 0, WHITE); y += 32;
                tbWidth.bounds  = (Rectangle) {propertiesArea.x + 20, y, 100, 28};
                tbHeight.bounds = (Rectangle) {propertiesArea.x + 130, y, 100, 28};
                textBoxDraw(&tbWidth, lobby_fonts[FONT18], 18);
                textBoxDraw(&tbHeight, lobby_fonts[FONT18], 18);
                y += 40;

                // Roundness
                DrawTextEx(lobby_fonts[FONT18], "Roundness", (Vector2) {propertiesArea.x + 20, y}, 18, 0, WHITE); y += 32;
                tbRoundness.bounds = (Rectangle) {propertiesArea.x + 20, y, 120, 28};
                sliderRoundness.bounds = (Rectangle) {propertiesArea.x + 150, y, 100, 28};
                textBoxDraw(&tbRoundness, lobby_fonts[FONT18], 18);
                sliderDraw(&sliderRoundness, lobby_fonts[FONT18], 18);
                y += 50;

                // Moving platform fields
                if (t->type == TERRAIN_MOVING_H || t->type == TERRAIN_MOVING_V) {
                    DrawTextEx(lobby_fonts[FONT18], "Moving Platform", (Vector2) {propertiesArea.x + 20, y}, 18, 0, ORANGE); y += 32;
                    tbVelX.bounds = (Rectangle) {propertiesArea.x + 20, y, 100, 28};
                    tbVelY.bounds = (Rectangle) {propertiesArea.x + 130, y, 100, 28};
                    textBoxDraw(&tbVelX, lobby_fonts[FONT18], 18);
                    textBoxDraw(&tbVelY, lobby_fonts[FONT18], 18);
                    y += 40;

                    tbMoveDist.bounds = (Rectangle) {propertiesArea.x + 20, y, 220, 28};
                    DrawTextEx(lobby_fonts[FONT18], "Distance", (Vector2) {propertiesArea.x + 20, y}, 18, 0, LIGHTGRAY);
                    textBoxDraw(&tbMoveDist, lobby_fonts[FONT18], 18);
                    y += 40;
                }

                f32 typeY = y;
                y += 50 + 32;

                // Portal fields
                if (t->type == TERRAIN_PORTAL) {
                    DrawTextEx(lobby_fonts[FONT18], "Portal Target", (Vector2) {propertiesArea.x + 20, y}, 18, 0, ORANGE); y += 32;
                    tbTargetX.bounds = (Rectangle) {propertiesArea.x + 20, y, 100, 28};
                    tbTargetY.bounds = (Rectangle) {propertiesArea.x + 130, y, 100, 28};
                    textBoxDraw(&tbTargetX, lobby_fonts[FONT18], 18);
                    textBoxDraw(&tbTargetY, lobby_fonts[FONT18], 18);
                    y += 40;

                    // Pick target button
                    btnPickTarget.bounds = (Rectangle){propertiesArea.x + 20, y, 220, 28};
                    textButtonDraw(&btnPickTarget, lobby_fonts[FONT18], 18);
                    y += 40;

                    // Two-way checkbox
                    cbTwoWay.bounds = (Rectangle){propertiesArea.x + 20, y, 24, 24};
                    checkBoxDraw(&cbTwoWay, lobby_fonts[FONT18], 18);
                    y += 40;

                    // Only-receiver checkbox
                    cbOnlyReceiver.bounds = (Rectangle){propertiesArea.x + 20, y, 24, 24};
                    checkBoxDraw(&cbOnlyReceiver, lobby_fonts[FONT18], 18);
                    y += 40;
                }

                // Grid snap toggle
                DrawTextEx(lobby_fonts[FONT18], "Grid Snap", (Vector2) {propertiesArea.x + 20, y}, 18, 0, WHITE); y += 32;
                static TextButton_St btnGridSnap = { .text = "ON", .baseColor = GREEN };
                btnGridSnap.bounds = (Rectangle) {propertiesArea.x + 20, y, 80, 28};
                if (propertiesGridSnapEnabled) btnGridSnap.baseColor = GREEN; else btnGridSnap.baseColor = RED;
                btnGridSnap.text = propertiesGridSnapEnabled ? "ON" : "OFF";
                textButtonDraw(&btnGridSnap, lobby_fonts[FONT18], 18);
                y += 50;

                // Type
                DrawTextEx(lobby_fonts[FONT18], "Type", (Vector2) {propertiesArea.x + 20, typeY}, 18, 0, WHITE); typeY += 32;
                dropdownType.bounds = (Rectangle) {propertiesArea.x + 20, typeY, 220, 28};
                dropdownDraw(&dropdownType, terrainTypeNames, __terrainTypeCount, lobby_fonts[FONT18], 18);

            } else {
                // ── MULTI-SELECT LIST MODE ─────────────────────────────────────────
                f32 listY = baseY;

                for (size_t i = 0; i < selectedIndices.count; ++i) {
                    s32 idx = selectedIndices.items[i];
                    if (idx < 0 || (size_t)idx >= terrains.count) continue;

                    const LobbyTerrain_St* t = &terrains.items[idx];

                    Rectangle itemRect = {
                        propertiesArea.x + 20.0f,
                        listY,
                        220.0f,
                        28.0f
                    };

                    TextButton_St listBtn = {
                        .bounds = itemRect,
                        .state = WIDGET_STATE_NORMAL,
                        .text = TextFormat("%s (%d)", terrainTypeNames[t->type], idx),
                        .baseColor = (idx == focusedTerrainIndex) ? SKYBLUE : LIGHTGRAY
                    };

                    textButtonDraw(&listBtn, lobby_fonts[FONT18], 18);

                    listY += 32.0f;
                }
            }

        } scrollFrameEnd();

        if (focusedTerrainIndex != -1 && selectedIndices.count > 1) {
            // Red X exit button (top-right of panel)
            btnExitFocus.bounds = (Rectangle) {
                propertiesArea.x - 10.0f + 220.0f,
                70.0f,
                30.0f, 30.0f
            };
            textButtonDraw(&btnExitFocus, lobby_fonts[FONT18], 24);
        }
    }

    // Paste anchor diagram
    if (clipboardHasData && editorDragMode == DRAG_NONE) {
        f32 diagramX = (f32)screenW - 100.0f;
        if (game->showPropertiesPanel) diagramX -= rightPanelWidth;

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

    // Help panel
    if (showHelpPanel) {
        const f32 panelW = 520.0f;
        const f32 panelH = 380.0f;
        const f32 panelX = ((f32)screenW - panelW) / 2.0f;
        const f32 panelY = ((f32)screenH - panelH) / 2.0f;

        DrawRectangle(panelX - 10, panelY - 10, panelW + 20, panelH + 20, Fade(BLACK, 0.85f));
        DrawRectangle(panelX, panelY, panelW, panelH, Fade(DARKGRAY, 0.95f));
        DrawRectangleLinesEx((Rectangle) {panelX, panelY, panelW, panelH}, 4.0f, YELLOW);

        DrawTextEx(lobby_fonts[FONT24], "EDITOR HELP - KEYBINDS",
                (Vector2) {panelX + 20, panelY + 15}, 24, 0, YELLOW);

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

        Rectangle helpArea = {panelX, panelY + 55.0f, panelW, panelH - 110.0f};
        helpScroll.contentHeight = (f32)ARRAY_LEN(lines) * 15.0f;

        scrollFrameBegin(&helpScroll, helpArea);{
            for (u32 i = 0; i < ARRAY_LEN(lines); ++i) {
                f32 y = 55.0f + (f32)i * 22.0f - helpScroll.scrollY;
                DrawTextEx(lobby_fonts[FONT18], lines[i],
                        (Vector2) {panelX + 30, y}, 18, 0, LIGHTGRAY);
            }
        } scrollFrameEnd();

        DrawTextEx(lobby_fonts[FONT18], "Press H to close",
                (Vector2) {panelX + 30, panelY + panelH - 30}, 18, 0, ORANGE);
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