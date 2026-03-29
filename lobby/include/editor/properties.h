/**
    @file editor/properties.h
    @author Fshimi-Hawlk
    @date 2026-03-28
    @date 2026-03-29
    @brief Public interface for the level editor properties panel (right sidebar).

    Uses the new reusable widget system (text boxes, sliders, dropdown, scroll frame, buttons).
*/

#ifndef EDITOR_PROPERTIES_H
#define EDITOR_PROPERTIES_H

#include "utils/userTypes.h"
#include "widgets/types.h"

extern TextBox_St   tbPosX, tbPosY, tbWidth, tbHeight, tbRoundness;
extern TextBox_St   tbVelX, tbVelY, tbMoveDist;
extern TextBox_St   tbTargetX, tbTargetY;
extern Slider_St    sliderRoundness;
extern DropDown_St  dropdownType;
extern TextButton_St btnExitFocus;
extern TextButton_St btnPickTarget;
extern CheckBox_St   cbTwoWay;

extern bool propertiesGridSnapEnabled;
extern f32  propertiesGridStep;

extern s32 focusedTerrainIndex;   ///< -1 = multi-select list mode

/**
    @brief Updates all editable properties from the right panel.
           Called every frame from `updateEditor()` when the panel is visible.
    @param game Pointer to the full lobby game state
*/
void updatePropertiesPanel(LobbyGame_St* const game);

/**
    @brief Synchronizes the property edit buffers / widget states with the currently selected terrain(s).
           Should be called whenever the selection changes.
    @param game Pointer to the lobby game state
*/
void refreshPropertyBuffers(const LobbyGame_St* const game);

/**
    @brief Handles mouse interaction inside the properties panel when in multi-select list mode.
           - Hovering a terrain name highlights the corresponding world terrain in lime.
           - Clicking a terrain name switches to single-terrain focused mode + camera focus.
    @param game        Pointer to the lobby game state
    @param mouseScreen Screen-space mouse position
*/
void handlePropertiesMultiSelectClick(LobbyGame_St* const game, Vector2 mouseScreen);

/**
    @brief Returns from single-terrain focused mode back to the multi-select list view.
           Restores normal yellow selection highlights.
    @param game Pointer to the lobby game state
*/
void exitSingleTerrainFocusMode(LobbyGame_St* const game);

void propertiesInit(void);

/**
    @brief Focuses the editor camera on a specific terrain (smooth zoom + centering).
    @param game Pointer to the lobby game state
    @param terrainIndex Index of the terrain to focus on
*/
void focusCameraOnTerrain(LobbyGame_St* const game, s32 terrainIndex);

#endif // EDITOR_PROPERTIES_H