/**
    @file properties.h
    @author Léandre BAUDET
    @date 2026-03-30
    @date 2026-04-14
    @brief Public interface for the level editor properties panel.
*/
#ifndef EDITOR_PROPERTIES_H
#define EDITOR_PROPERTIES_H

#include "utils/userTypes.h"
#include "sharedWidgets/types.h"

/**
    @brief Global variable tbRoundness
*/
extern TextBox_St   tbPosX, tbPosY, tbWidth, tbHeight, tbRoundness;
/**
    @brief Global variable tbMoveDist
*/
extern TextBox_St   tbVelX, tbVelY, tbMoveDist;
/**
    @brief Global variable tbTargetY
*/
extern TextBox_St   tbTargetX, tbTargetY;
/**
    @brief Global variable sliderRoundness
*/
extern Slider_St    sliderRoundness;
/**
    @brief Global variable dropdownType
*/
/**
    @brief Global variable btnExitFocus
*/
extern DropDown_St  dropdownKind;
extern TextButton_St btnExitFocus;
/**
    @brief Global variable btnPickTarget
*/
extern TextButton_St btnPickTarget;
/**
    @brief Global variable cbTwoWay
*/
extern CheckBox_St   cbTwoWay;
/**
    @brief Global variable cbOnlyReceiver
*/
extern CheckBox_St   cbOnlyReceiver;

/**
    @brief Global variable propertiesGridSnapEnabled
*/
extern bool propertiesGridSnapEnabled;
/**
    @brief Global variable propertiesGridStep
*/
extern f32  propertiesGridStep;

/**
    @brief Global variable focusedTerrainIndex
*/
extern s32 focusedTerrainIndex;

/**
    @brief Description for updatePropertiesPanel
    @param[in,out] game The game parameter
*/
void updatePropertiesPanel(LobbyGame_St* const game);
/**
    @brief Description for refreshPropertyBuffers
    @param[in,out] game The game parameter
*/
void refreshPropertyBuffers(const LobbyGame_St* const game);
/**
    @brief Description for handlePropertiesMultiSelectClick
    @param[in,out] game The game parameter
    @param[in,out] mouseScreen The mouseScreen parameter
*/
void handlePropertiesMultiSelectClick(LobbyGame_St* const game, Vector2 mouseScreen);
/**
    @brief Description for exitSingleTerrainFocusMode
    @param[in,out] game The game parameter
*/
void exitSingleTerrainFocusMode(LobbyGame_St* const game);
/**
    @brief Description for propertiesInit
*/
void propertiesInit(void);
/**
    @brief Description for focusCameraOnTerrain
    @param[in,out] game The game parameter
    @param[in,out] terrainIndex The terrainIndex parameter
*/
void focusCameraOnTerrain(LobbyGame_St* const game, s32 terrainIndex);

#endif // EDITOR_PROPERTIES_H
