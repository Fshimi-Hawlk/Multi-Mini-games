/**
    @file editor/input.c
    @author Fshimi-Hawlk
    @date 2026-03-30
    @brief Implementation of the level editor input handling with integrated widgets.
*/

#include "editor/editor.h"
#include "editor/types.h"
#include "editor/properties.h"
#include "editor/utils.h"
#include "utils/globals.h"
#include "utils/utils.h"
#include "widgets/button.h"
#include "widgets/scrollFrame.h"
#include "widgets/checkBox.h"
#include <math.h>
#include <stdlib.h>
#include <stdio.h>

void updateEditor(LobbyGame_St* game, float dt) {
    if (!game->editorMode) return;
    (void)dt;

    Vector2 mouseScreen = GetMousePosition();
    Vector2 mouseWorld = getMouseWorld(game);

    // Update UI buttons
    textButtonUpdate(&btnLoad, mouseScreen);
    textButtonUpdate(&btnSave, mouseScreen);
    textButtonUpdate(&btnGenerate, mouseScreen);

    // Update scroll frames
    scrollFrameUpdate(&paletteScroll, mouseScreen);
    scrollFrameUpdate(&propertiesScroll, mouseScreen);
    
    // UI check
    bool onUI = (mouseScreen.x < 220.0f) || 
                (game->showPropertiesPanel && mouseScreen.x > GetScreenWidth() - 260.0f);

    if (game->showPropertiesPanel) {
        updatePropertiesPanel(game);
    }

    // Keyboard Shortcuts 
    
    if (IsKeyPressed(KEY_G)) game->showGrid = !game->showGrid;

    // Delete selected
    if (IsKeyPressed(KEY_DELETE) || IsKeyPressed(KEY_BACKSPACE)) {
        if (!editorAnyTextBoxActive && selectedIndices.count > 0) {
            // Remove from terrains (reverse order to keep indices valid)
            for (int i = (int)selectedIndices.count - 1; i >= 0; --i) {
                da_remove_unordered(&terrains, selectedIndices.items[i]);
            }
            da_clear(&selectedIndices);
            game->selectedTerrainIndex = -1;
            refreshPropertyBuffers(game);
        }
    }

    // Mouse Interaction 

    // Panning (Right click)
    if (IsMouseButtonDown(MOUSE_RIGHT_BUTTON)) {
        Vector2 delta = GetMouseDelta();
        game->cam.target.x -= delta.x / game->cam.zoom;
        game->cam.target.y -= delta.y / game->cam.zoom;
    }

    // Zoom (Wheel)
    float wheel = GetMouseWheelMove();
    if (wheel != 0) {
        Vector2 mouseWorldBefore = getMouseWorld(game);
        game->cam.zoom += wheel * 0.1f;
        if (game->cam.zoom < 0.1f) game->cam.zoom = 0.1f;
        Vector2 mouseWorldAfter = getMouseWorld(game);
        game->cam.target.x += (mouseWorldBefore.x - mouseWorldAfter.x);
        game->cam.target.y += (mouseWorldBefore.y - mouseWorldAfter.y);
    }

    // Left click interaction
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && !onUI) {
        s32 hit = findTerrainAtPoint(mouseWorld);
        
        if (portalTargetPickMode) {
            if (portalBeingConfigured != -1) {
                terrains.items[portalBeingConfigured].portalTargetPosition = mouseWorld;
                portalTargetPickMode = false;
                portalBeingConfigured = -1;
                refreshPropertyBuffers(game);
            }
        }
        else if (hit != -1) {
            // Selection logic
            bool shift = IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT);
            if (!shift) da_clear(&selectedIndices);
            
            bool alreadySelected = false;
            for(size_t i=0; i<selectedIndices.count; i++) {
                if (selectedIndices.items[i] == hit) {
                    alreadySelected = true;
                    break;
                }
            }
            if (!alreadySelected) da_append(&selectedIndices, hit);
            
            game->selectedTerrainIndex = hit;
            editorDragMode = DRAG_MOVING;
            dragStartWorld = mouseWorld;
            refreshPropertyBuffers(game);
        }
        else {
            // Clicked on empty space: Start placing new or deselect
            bool shift = IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT);
            if (!shift) {
                da_clear(&selectedIndices);
                game->selectedTerrainIndex = -1;
                refreshPropertyBuffers(game);
                
                // Start placing new terrain
                LobbyTerrain_St nt = createDefaultTerrain(currentPaletteType, mouseWorld);
                da_append(&terrains, nt);
                game->selectedTerrainIndex = (s32)terrains.count - 1;
                da_append(&selectedIndices, game->selectedTerrainIndex);
                editorDragMode = DRAG_MOVING;
                dragStartWorld = mouseWorld;
                refreshPropertyBuffers(game);
            }
        }
    }

    // Dragging logic
    if (IsMouseButtonDown(MOUSE_LEFT_BUTTON) && editorDragMode == DRAG_MOVING) {
        Vector2 currentMouseWorld = getMouseWorld(game);
        Vector2 delta = { currentMouseWorld.x - dragStartWorld.x, currentMouseWorld.y - dragStartWorld.y };
        
        for (size_t i = 0; i < selectedIndices.count; ++i) {
            s32 idx = selectedIndices.items[i];
            if (idx >= 0 && (size_t)idx < terrains.count) {
                terrains.items[idx].rect.x += delta.x;
                terrains.items[idx].rect.y += delta.y;
            }
        }
        dragStartWorld = currentMouseWorld;
    }

    if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
        editorDragMode = DRAG_NONE;
    }
}
