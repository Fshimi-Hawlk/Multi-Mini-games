/**
 * @file menus.c
 * @brief Implémentation du système de menus.
 */

#include "ui/menus.h"
#include "firstparty/systemSettings.h"
#include "firstparty/leaderboard.h"
#include <stddef.h>

MenuType_Et g_currentMenu = MENU_MAIN;

void InitMenus(void) {
    g_currentMenu = MENU_MAIN;
}

void UpdateMenu(void) {
    if (IsKeyPressed(KEY_ESCAPE)) {
        if (g_currentMenu == MENU_NONE) g_currentMenu = MENU_PAUSE;
        else if (g_currentMenu == MENU_PAUSE) g_currentMenu = MENU_NONE;
        else if (g_currentMenu == MENU_SETTINGS) g_currentMenu = MENU_PAUSE;
        else if (g_currentMenu == MENU_LEADERBOARD) g_currentMenu = MENU_MAIN;
    }

    switch (g_currentMenu) {
        case MENU_MAIN:        UpdateMainMenu(); break;
        case MENU_PAUSE:       UpdatePauseMenu(); break;
        case MENU_SETTINGS:    UpdateSettingsMenu(); break;
        case MENU_LEADERBOARD: UpdateLeaderboardMenu(); break;
        default: break;
    }
}

void DrawMenu(void) {
    switch (g_currentMenu) {
        case MENU_MAIN:        DrawMainMenu(); break;
        case MENU_PAUSE:       DrawPauseMenu(); break;
        case MENU_SETTINGS:    DrawSettingsMenu(); break;
        case MENU_LEADERBOARD: DrawLeaderboardMenu(); break;
        default: break;
    }
}

void UpdateMainMenu(void) {
    Vector2 m = GetMousePosition();
    int sw = GetScreenWidth();
    int sh = GetScreenHeight();
    
    Rectangle btnPlay = { sw/2 - 100, sh/2 - 50, 200, 50 };
    Rectangle btnLB   = { sw/2 - 100, sh/2 + 20, 200, 50 };
    Rectangle btnSet  = { sw/2 - 100, sh/2 + 90, 200, 50 };
    Rectangle btnExit = { sw/2 - 100, sh/2 + 160, 200, 50 };
    
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        if (CheckCollisionPointRec(m, btnPlay)) g_currentMenu = MENU_NONE;
        if (CheckCollisionPointRec(m, btnLB))   g_currentMenu = MENU_LEADERBOARD;
        if (CheckCollisionPointRec(m, btnSet))  g_currentMenu = MENU_SETTINGS;
        if (CheckCollisionPointRec(m, btnExit)) CloseWindow();
    }
}

void DrawMainMenu(void) {
    int sw = GetScreenWidth();
    int sh = GetScreenHeight();
    DrawRectangleGradientV(0, 0, sw, sh, DARKBLUE, BLACK);
    
    DrawText("MULTI MINI-GAMES", sw/2 - MeasureText("MULTI MINI-GAMES", 50)/2, sh/4, 50, GOLD);
    
    char* labels[] = {"PLAY", "LEADERBOARD", "SETTINGS", "EXIT"};
    for (int i=0; i<4; i++) {
        Rectangle r = { sw/2 - 100, sh/2 - 50 + i*70, 200, 50 };
        bool hover = CheckCollisionPointRec(GetMousePosition(), r);
        DrawRectangleRounded(r, 0.2f, 10, hover ? LIGHTGRAY : GRAY);
        DrawText(labels[i], r.x + 100 - MeasureText(labels[i], 20)/2, r.y + 15, 20, hover ? BLACK : WHITE);
    }
}

void UpdatePauseMenu(void) {
    Vector2 m = GetMousePosition();
    int sw = GetScreenWidth();
    int sh = GetScreenHeight();
    
    Rectangle btnResume = { sw/2 - 100, sh/2 - 50, 200, 50 };
    Rectangle btnSet    = { sw/2 - 100, sh/2 + 20, 200, 50 };
    Rectangle btnExit   = { sw/2 - 100, sh/2 + 90, 200, 50 };

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        if (CheckCollisionPointRec(m, btnResume)) g_currentMenu = MENU_NONE;
        if (CheckCollisionPointRec(m, btnSet))    g_currentMenu = MENU_SETTINGS;
        if (CheckCollisionPointRec(m, btnExit)) {
            extern void switch_minigame(u8 game_id);
            switch_minigame(0);
            g_currentMenu = MENU_NONE;
        }
    }
}

void DrawPauseMenu(void) {
    int sw = GetScreenWidth();
    int sh = GetScreenHeight();
    DrawRectangle(0, 0, sw, sh, Fade(BLACK, 0.6f));
    DrawRectangleRounded((Rectangle){sw/2 - 150, sh/2 - 100, 300, 300}, 0.1f, 10, DARKGRAY);
    DrawText("PAUSE", sw/2 - MeasureText("PAUSE", 30)/2, sh/2 - 80, 30, WHITE);
    
    char* labels[] = {"RESUME", "SETTINGS", "ABANDON"};
    for (int i=0; i<3; i++) {
        Rectangle r = { sw/2 - 100, sh/2 - 50 + i*70, 200, 50 };
        bool hover = CheckCollisionPointRec(GetMousePosition(), r);
        DrawRectangleRounded(r, 0.2f, 10, hover ? LIGHTGRAY : GRAY);
        DrawText(labels[i], r.x + 100 - MeasureText(labels[i], 20)/2, r.y + 15, 20, (i == 2) ? MAROON : (hover ? BLACK : WHITE));
    }
}

void UpdateSettingsMenu(void) {
    if (IsKeyPressed(KEY_ESCAPE)) g_currentMenu = MENU_PAUSE;
    if (IsKeyPressed(KEY_M)) systemSettings.audio.musicVolume = (systemSettings.audio.musicVolume + 0.1f);
    if (systemSettings.audio.musicVolume > 1.0f) systemSettings.audio.musicVolume = 0.0f;
    if (IsKeyPressed(KEY_F)) {
        ToggleFullscreen();
        systemSettings.video.fullscreen = !systemSettings.video.fullscreen;
    }
}

void DrawSettingsMenu(void) {
    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(BLACK, 0.8f));
    DrawText("SETTINGS", GetScreenWidth()/2 - 100, 100, 40, WHITE);
    DrawText(TextFormat("Music Volume: %.1f  (M)", systemSettings.audio.musicVolume), 100, 200, 25, LIGHTGRAY);
    DrawText(TextFormat("Fullscreen: %s (F)", systemSettings.video.fullscreen ? "ON" : "OFF"), 100, 250, 25, LIGHTGRAY);
    DrawText("ESC pour retour", 100, GetScreenHeight() - 50, 20, GRAY);
}

void UpdateLeaderboardMenu(void) {
    if (IsKeyPressed(KEY_ESCAPE)) g_currentMenu = MENU_MAIN;
}

void DrawLeaderboardMenu(void) {
    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(BLACK, 0.9f));
    DrawText("TOP 10 - KING FOR FOUR", GetScreenWidth()/2 - 200, 80, 40, GOLD);
    
    Leaderboard_St lb = LoadLeaderboard(1);
    int startY = 160;
    for (int i=0; i<10 && i<lb.count; i++) {
        DrawText(TextFormat("%d. %s", i+1, lb.entries[i].name), 150, startY + i*35, 25, (i == 0) ? GOLD : WHITE);
        DrawText(TextFormat("%d pts", lb.entries[i].score), GetScreenWidth() - 350, startY + i*35, 25, (i == 0) ? GOLD : WHITE);
    }
    DrawText("ESC pour retour", GetScreenWidth()/2 - 100, GetScreenHeight() - 50, 20, GRAY);
}
