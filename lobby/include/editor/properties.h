/**
    @file editor/properties.h
    @author Fshimi-Hawlk
    @date 2026-03-30
    @brief Public interface for the level editor properties panel.
*/

#ifndef EDITOR_PROPERTIES_H
#define EDITOR_PROPERTIES_H

#include "utils/userTypes.h"
#include "sharedWidgets/types.h"

extern TextBox_St   tbPosX, tbPosY, tbWidth, tbHeight, tbRoundness;
extern TextBox_St   tbVelX, tbVelY, tbMoveDist;
extern TextBox_St   tbTargetX, tbTargetY;
extern Slider_St    sliderRoundness;
extern DropDown_St  dropdownKind;
extern TextButton_St btnExitFocus;
extern TextButton_St btnPickTarget;
extern CheckBox_St   cbTwoWay;
extern CheckBox_St   cbOnlyReceiver;

extern bool propertiesGridSnapEnabled;
extern f32  propertiesGridStep;

extern s32 focusedTerrainIndex;

void updatePropertiesPanel(LobbyGame_St* const game);
void refreshPropertyBuffers(const LobbyGame_St* const game);
void handlePropertiesMultiSelectClick(LobbyGame_St* const game, Vector2 mouseScreen);
void exitSingleTerrainFocusMode(LobbyGame_St* const game);
void propertiesInit(void);
void focusCameraOnTerrain(LobbyGame_St* const game, s32 terrainIndex);

#endif // EDITOR_PROPERTIES_H
