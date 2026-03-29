/**
    @file editor/properties.c
    @author Fshimi-Hawlk
    @date 2026-03-28
    @date 2026-03-29
    @brief Full implementation of the level editor properties panel using the reusable widget system.

    Contributors:
        - Fshimi-Hawlk:
            - Complete widget-based property editing
            - Multi-select list with TextButton_St + hover/click
            - Single-terrain focus mode with red "X" exit button
            - All fields (position, size, roundness, type, moving, portal)
            - Grid snap toggle + step selector
            - Live preview + immediate application of changes

    @see `editor/properties.h`
    @see `widgets/types.h`
*/

#include "editor/types.h"
#include "editor/editor.h"
#include "editor/properties.h"
#include "editor/utils.h"

#include "widgets/textBox.h"
#include "widgets/button.h"
#include "widgets/slider.h"
#include "widgets/dropdown.h"
#include "widgets/scrollFrame.h"
#include "widgets/checkBox.h"

#include "utils/globals.h"
#include "utils/utils.h"

#include "systemSettings.h"

// ── Local widget instances ──────────────────────────────────────────────────

TextBox_St   tbPosX, tbPosY, tbWidth, tbHeight, tbRoundness;
TextBox_St   tbVelX, tbVelY, tbMoveDist;
TextBox_St   tbTargetX, tbTargetY;

Slider_St    sliderRoundness;
DropDown_St  dropdownType;
TextButton_St btnExitFocus;

TextButton_St btnPickTarget;
CheckBox_St   cbTwoWay;

bool propertiesGridSnapEnabled = true;
f32  propertiesGridStep        = 25.0f;

s32  focusedTerrainIndex       = -1;   // -1 = multi-select list mode

// ── Helper functions ────────────────────────────────────────────────────────

static void refreshSingleTerrainBuffers(s32 idx) {
    if (idx < 0 || (size_t)idx >= terrains.count) {
        tbPosX.buffer[0] = tbPosY.buffer[0] = tbWidth.buffer[0] = tbHeight.buffer[0] = '\0';
        tbRoundness.buffer[0] = tbVelX.buffer[0] = tbVelY.buffer[0] = tbMoveDist.buffer[0] = '\0';
        tbTargetX.buffer[0] = tbTargetY.buffer[0] = '\0';
        sliderRoundness.value = 0.0f;
        return;
    }

    const LobbyTerrain_St* t = &terrains.items[idx];

    snprintf(tbPosX.buffer,     sizeof(tbPosX.buffer),     "%.1f", t->rect.x);
    snprintf(tbPosY.buffer,     sizeof(tbPosY.buffer),     "%.1f", t->rect.y);
    snprintf(tbWidth.buffer,    sizeof(tbWidth.buffer),    "%.1f", t->rect.width);
    snprintf(tbHeight.buffer,   sizeof(tbHeight.buffer),   "%.1f", t->rect.height);
    snprintf(tbRoundness.buffer,sizeof(tbRoundness.buffer),"%.2f", t->roundness);

    sliderRoundness.value = t->roundness;

    if (t->type == TERRAIN_MOVING_H || t->type == TERRAIN_MOVING_V) {
        snprintf(tbVelX.buffer, sizeof(tbVelX.buffer), "%.1f", t->velocity.x);
        snprintf(tbVelY.buffer, sizeof(tbVelY.buffer), "%.1f", t->velocity.y);
        snprintf(tbMoveDist.buffer, sizeof(tbMoveDist.buffer), "%.1f", t->moveDistance);
    } else {
        tbVelX.buffer[0] = tbVelY.buffer[0] = tbMoveDist.buffer[0] = '\0';
    }

    if (t->type == TERRAIN_PORTAL) {
        snprintf(tbTargetX.buffer, sizeof(tbTargetX.buffer), "%.1f", t->portalTargetPosition.x);
        snprintf(tbTargetY.buffer, sizeof(tbTargetY.buffer), "%.1f", t->portalTargetPosition.y);
    } else {
        tbTargetX.buffer[0] = tbTargetY.buffer[0] = '\0';
    }

    // Sync dropdown
    dropdownType.selectedIndex = (s32)t->type;
}

static void applyPropertyChanges(LobbyTerrain_St* t) {
    if (tbPosX.buffer[0])  t->rect.x      = snapToGrid(atof(tbPosX.buffer));
    if (tbPosY.buffer[0])  t->rect.y      = snapToGrid(atof(tbPosY.buffer));
    if (tbWidth.buffer[0]) t->rect.width  = atof(tbWidth.buffer);
    if (tbHeight.buffer[0])t->rect.height = atof(tbHeight.buffer);
    if (tbRoundness.buffer[0]) t->roundness = clamp(atof(tbRoundness.buffer), 0.0f, 1.0f);

    if (t->type == TERRAIN_MOVING_H || t->type == TERRAIN_MOVING_V) {
        if (tbVelX.buffer[0]) t->velocity.x = atof(tbVelX.buffer);
        if (tbVelY.buffer[0]) t->velocity.y = atof(tbVelY.buffer);
        if (tbMoveDist.buffer[0]) t->moveDistance = atof(tbMoveDist.buffer);
    }

    if (t->type == TERRAIN_PORTAL) {
        if (tbTargetX.buffer[0]) t->portalTargetPosition.x = atof(tbTargetX.buffer);
        if (tbTargetY.buffer[0]) t->portalTargetPosition.y = atof(tbTargetY.buffer);
    }
}

// ── Public API ──────────────────────────────────────────────────────────────

void refreshPropertyBuffers(const LobbyGame_St* const game) {
    UNUSED(game);

    if (focusedTerrainIndex != -1) {
        refreshSingleTerrainBuffers(focusedTerrainIndex);
    } else if (selectedIndices.count == 1) {
        refreshSingleTerrainBuffers(selectedIndices.items[0]);
    } else {
        // multi-select → clear edit fields
        tbPosX.buffer[0] = tbPosY.buffer[0] = tbWidth.buffer[0] = tbHeight.buffer[0] = '\0';
        tbRoundness.buffer[0] = tbVelX.buffer[0] = tbVelY.buffer[0] = tbMoveDist.buffer[0] = '\0';
        tbTargetX.buffer[0] = tbTargetY.buffer[0] = '\0';
        sliderRoundness.value = 0.0f;
    }
}

void focusCameraOnTerrain(LobbyGame_St* const game, s32 terrainIndex) {
    if (terrainIndex < 0 || (size_t)terrainIndex >= terrains.count) return;

    const Rectangle r = terrains.items[terrainIndex].rect;
    game->cam.target.x = r.x + r.width  * 0.5f;
    game->cam.target.y = r.y + r.height * 0.5f;

    f32 zoomX = systemSettings.video.width  / (r.width  * 1.8f);
    f32 zoomY = systemSettings.video.height / (r.height * 1.8f);
    game->cam.zoom = min(zoomX, zoomY);
    game->cam.zoom = clamp(game->cam.zoom, 0.4f, 4.0f);
}

void exitSingleTerrainFocusMode(LobbyGame_St* const game) {
    UNUSED(game);
    focusedTerrainIndex = -1;
}

void handlePropertiesMultiSelectClick(LobbyGame_St* const game, Vector2 mouseScreen) {
    if (focusedTerrainIndex != -1 || selectedIndices.count <= 1) return;

    f32 startY = 140.0f;   // after title
    f32 lineHeight = 28.0f;

    for (size_t i = 0; i < selectedIndices.count; ++i) {
        s32 idx = selectedIndices.items[i];
        if (idx < 0 || (size_t)idx >= terrains.count) continue;

        Rectangle itemRect = {
            systemSettings.video.width - 260.0f + 20.0f,
            startY + i * lineHeight - propertiesScroll.scrollY,
            220.0f,
            lineHeight
        };

        if (CheckCollisionPointRec(mouseScreen, itemRect)) {
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                focusedTerrainIndex = idx;
                refreshPropertyBuffers(game);
                focusCameraOnTerrain(game, idx);
            }
            return;
        }
    }
}

void updatePropertiesPanel(LobbyGame_St* const game) {
    if (!game->showPropertiesPanel) return;

    Vector2 mouse = GetMousePosition();
    Rectangle propArea = {
        systemSettings.video.width - 260.0f,
        50.0f,
        260.0f,
        systemSettings.video.height - 80.0f
    };

    scrollFrameUpdate(&propertiesScroll, propArea, mouse);

    if (focusedTerrainIndex != -1 || selectedIndices.count == 1) {
        // ── SINGLE TERRAIN EDIT MODE ───────────────────────────────────────

        s32 idx = (focusedTerrainIndex != -1) ? focusedTerrainIndex : selectedIndices.items[0];
        if (idx < 0 || (size_t) idx >= terrains.count) {
            exitSingleTerrainFocusMode(game);
            return;
        }

        LobbyTerrain_St* t = &terrains.items[idx];

        propertiesBeingEdited = false;

        // Update widgets
        if (textBoxUpdate(&tbPosX, mouse))      applyPropertyChanges(t);
        if (textBoxUpdate(&tbPosY, mouse))      applyPropertyChanges(t);
        if (textBoxUpdate(&tbWidth, mouse))     applyPropertyChanges(t);
        if (textBoxUpdate(&tbHeight, mouse))    applyPropertyChanges(t);
        if (textBoxUpdate(&tbRoundness, mouse)) applyPropertyChanges(t);

        propertiesBeingEdited = propertiesBeingEdited || tbPosX.editMode || tbPosY.editMode || tbWidth.editMode || tbHeight.editMode || tbRoundness.editMode;

        if (sliderUpdate(&sliderRoundness, mouse)) {
            t->roundness = sliderRoundness.value;
            snprintf(tbRoundness.buffer, sizeof(tbRoundness.buffer), "%.2f", t->roundness);
        }

        if (dropdownUpdate(&dropdownType, mouse, __terrainTypeCount)) {
            t->type = (TerrainType_Et)dropdownType.selectedIndex;
            refreshSingleTerrainBuffers(idx);   // refresh type-specific fields
        }

        propertiesBeingEdited = propertiesBeingEdited || dropdownType.isOpen;

        if (textBoxUpdate(&tbVelX, mouse) || textBoxUpdate(&tbVelY, mouse) || textBoxUpdate(&tbMoveDist, mouse)) {
            applyPropertyChanges(t);
        }

        propertiesBeingEdited = propertiesBeingEdited || tbVelX.editMode || tbVelY.editMode || tbMoveDist.editMode;

        if (textBoxUpdate(&tbTargetX, mouse) || textBoxUpdate(&tbTargetY, mouse)) {
            applyPropertyChanges(t);
        }

        propertiesBeingEdited = propertiesBeingEdited || tbTargetX.editMode || tbTargetY.editMode;

        if (t->type == TERRAIN_PORTAL) {
            // Pick target button
            if (textButtonUpdate(&btnPickTarget, mouse)) {
                portalTargetPickMode = true;
                portalBeingConfigured = idx;
            }

            // Two-way checkbox
            if (checkBoxUpdate(&cbTwoWay, mouse)) {
                // If we just enabled two-way, try to make the link mutual
                if (cbTwoWay.checked) {
                    // (logic in input.c will handle on next click)
                }
            }
        }

        propertiesBeingEdited = propertiesBeingEdited || portalTargetPickMode;

        // Red X button
        if (textButtonUpdate(&btnExitFocus, mouse)) {
            exitSingleTerrainFocusMode(game);
            refreshPropertyBuffers(game);
        }

    } else if (selectedIndices.count > 1) {
        handlePropertiesMultiSelectClick(game, mouse);
    }
}

// ── Initialization (call from initEditor) ───────────────────────────────────

void propertiesInit(void) {
    scrollFrameInit(&propertiesScroll, 40.0f);

    // Text boxes
    tbPosX.bounds     = (Rectangle) {0,0,180,28};
    tbPosX.state = WIDGET_STATE_NORMAL;

    tbPosY.bounds     = (Rectangle) {0,0,180,28};
    tbPosY.state = WIDGET_STATE_NORMAL;

    tbWidth.bounds    = (Rectangle) {0,0,180,28};
    tbWidth.state = WIDGET_STATE_NORMAL;

    tbHeight.bounds   = (Rectangle) {0,0,180,28};
    tbHeight.state = WIDGET_STATE_NORMAL;

    tbRoundness.bounds= (Rectangle) {0,0,180,28};
    tbRoundness.state = WIDGET_STATE_NORMAL;

    tbVelX.bounds     = (Rectangle) {0,0,180,28};
    tbVelY.bounds     = (Rectangle) {0,0,180,28};
    tbMoveDist.bounds = (Rectangle) {0,0,180,28};
    tbTargetX.bounds  = (Rectangle) {0,0,180,28};
    tbTargetY.bounds  = (Rectangle) {0,0,180,28};

    // Slider
    sliderRoundness.bounds = (Rectangle) {0,0,180,24};
    sliderRoundness.minValue = 0.0f;
    sliderRoundness.maxValue = 1.0f;
    sliderRoundness.value = 0.0f;

    // Dropdown
    dropdownType.bounds = (Rectangle) {0,0,180,28};
    dropdownType.selectedIndex = 0;
    dropdownType.isOpen = false;

    // Red X button
    btnExitFocus.bounds = (Rectangle) {0,0,30,30};
    btnExitFocus.baseColor = RED;
    btnExitFocus.text = "X";
    btnExitFocus.state = WIDGET_STATE_NORMAL;

    // Portal target button
    btnPickTarget.bounds = (Rectangle){0,0,220,28};
    btnPickTarget.baseColor = SKYBLUE;
    btnPickTarget.text = "Pick Target Location";
    btnPickTarget.state = WIDGET_STATE_NORMAL;

    // Two-way checkbox
    cbTwoWay.bounds = (Rectangle){0,0,24,24};
    cbTwoWay.checked = false;
    cbTwoWay.label = "Two-way";
    cbTwoWay.state = WIDGET_STATE_NORMAL;
}