/**
 * @file menus.h
 * @brief Système de menus du lobby (Principal, Pause, Paramètres).
 */

#ifndef MENUS_H
#define MENUS_H

#include "raylib.h"
#include "firstparty/APIs/generalAPI.h"

typedef enum {
    MENU_NONE,
    MENU_MAIN,
    MENU_PAUSE,
    MENU_SETTINGS,
    MENU_LEADERBOARD
} MenuType_Et;

extern MenuType_Et g_currentMenu;

void InitMenus(void);
void UpdateMenu(void);
void DrawMenu(void);

// Utils de rendu
void DrawMainMenu(void);
void DrawPauseMenu(void);
void DrawSettingsMenu(void);
void DrawLeaderboardMenu(void);

// Logique
void UpdateMainMenu(void);
void UpdatePauseMenu(void);
void UpdateSettingsMenu(void);
void UpdateLeaderboardMenu(void);

#endif
