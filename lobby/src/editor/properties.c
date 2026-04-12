/**
    @file editor/properties.c
    @author Fshimi-Hawlk
    @date 2026-03-30
    @brief Implementation of the level editor properties panel logic.
*/

#include "editor/properties.h"

#include "utils/globals.h"

#include "sharedWidgets/button.h"
#include "sharedWidgets/textBox.h"
#include "sharedWidgets/dropdown.h"

TextBox_St   tbPosX, tbPosY, tbWidth, tbHeight, tbRoundness;
TextBox_St   tbVelX, tbVelY, tbMoveDist;
TextBox_St   tbTargetX, tbTargetY;
Slider_St    sliderRoundness;
Dropdown_St  dropdownType;
TextButton_St btnExitFocus;
TextButton_St btnPickTarget;
CheckBox_St   cbTwoWay;
CheckBox_St   cbOnlyReceiver;

bool propertiesGridSnapEnabled = true;
f32  propertiesGridStep = 25.0f;
s32  focusedTerrainIndex = -1;

static const char* terrainTypeNames[] = {
    "NORMAL", "WOOD", "STONE", "ICE", "BOUNCY", "MOV_H", "MOV_V", "WATER", "DECOR", "PORTAL"
};

void propertiesInit(void) {
    float x = (float)GetScreenWidth() - 280.0f;
    
    tbPosX = (TextBox_St){ .bounds = {x, 100, 100, 30}, .isValid = true };
    tbPosY = (TextBox_St){ .bounds = {x + 110, 100, 100, 30}, .isValid = true };
    
    dropdownType = (Dropdown_St){
        .bounds = {x, 180, 210, 30},
        .options = terrainTypeNames,
        .count = __terrainTypeCount,
        .selectedIndex = 0
    };
    
    btnExitFocus = (TextButton_St){ .bounds = {x, 500, 210, 40}, .baseColor = RED, .roundness = 0.2f };
    strncpy(btnExitFocus.text, "Back to List", 63);
}

void refreshPropertyBuffers(const LobbyGame_St* const game) {
    (void)game;
    if (focusedTerrainIndex == -1 || focusedTerrainIndex >= (s32)terrains.count) return;
    LobbyTerrain_St* t = &terrains.items[focusedTerrainIndex];
    
    snprintf(tbPosX.buffer, 255, "%.1f", t->rect.x);
    snprintf(tbPosY.buffer, 255, "%.1f", t->rect.y);
    dropdownType.selectedIndex = (s32)t->type;
}

void updatePropertiesPanel(LobbyGame_St* const game) {
    Vector2 m = GetMousePosition();
    
    if (focusedTerrainIndex != -1) {
        if (textBoxUpdate(&tbPosX, m)) terrains.items[focusedTerrainIndex].rect.x = (float)atof(tbPosX.buffer);
        if (textBoxUpdate(&tbPosY, m)) terrains.items[focusedTerrainIndex].rect.y = (float)atof(tbPosY.buffer);
        
        if (dropdownUpdate(&dropdownType, m)) {
            terrains.items[focusedTerrainIndex].type = (TerrainType_Et)dropdownType.selectedIndex;
        }
        
        if (textButtonUpdate(&btnExitFocus, m)) exitSingleTerrainFocusMode(game);
    }
}

void exitSingleTerrainFocusMode(LobbyGame_St* const game) {
    (void)game;
    focusedTerrainIndex = -1;
}

void focusCameraOnTerrain(LobbyGame_St* const game, s32 terrainIndex) {
    if (terrainIndex < 0 || terrainIndex >= (s32)terrains.count) return;
    focusedTerrainIndex = terrainIndex;
    refreshPropertyBuffers(game);
}
