/**
    @file ui/menus.h
    @author i-Charlys (CAILLON Charles)
    @author Fshimi-Hawlk
    @date 2026-04-13
    @date 2026-04-13
    @brief Menu system for the lobby (Main, Play Choice, Pause, Settings, Leaderboard).
*/

#ifndef UI_MENUS_H
#define UI_MENUS_H

#include "utils/userTypes.h"

typedef enum {
    MENU_NONE,
    MENU_MAIN,
    MENU_PAUSE,
    MENU_SETTINGS,
    MENU_LEADERBOARD,
    MENU_PLAY_CHOICE
} MenuType_Et;

extern MenuType_Et lobby_currentMenu;

void lobby_initMenus(void);
void lobby_updateMenu(void);
void lobby_drawMenu(void);

void lobby_updateMainMenu(void);
void lobby_updatePlayChoiceMenu(void);
void lobby_updatePauseMenu(void);
void lobby_updateSettingsMenu(void);
void lobby_updateLeaderboardMenu(void);

void lobby_drawMainMenu(void);
void lobby_drawPlayChoiceMenu(void);
void lobby_drawPauseMenu(void);
void lobby_drawSettingsMenu(void);
void lobby_drawLeaderboardMenu(void);

#endif