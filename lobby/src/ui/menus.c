/**
 * @file menus.c
 * @brief Implémentation du système de menus.
 */

#include "ui/menus.h"
#include "firstparty/systemSettings.h"
#include "firstparty/leaderboard.h"
#include "utils/userTypes.h"
#include <stddef.h>

MenuType_Et g_currentMenu = MENU_MAIN;
MenuType_Et g_previousMenu = MENU_MAIN;
extern LobbyGame_St lobby_game;

void InitMenus(void) {
    g_currentMenu = MENU_MAIN;
    g_previousMenu = MENU_MAIN;
}

void UpdateMenu(void) {
    if (IsKeyPressed(KEY_ESCAPE)) {
        if (g_currentMenu == MENU_NONE) {
            g_previousMenu = MENU_NONE;
            g_currentMenu = MENU_PAUSE;
        }
        else if (g_currentMenu == MENU_PAUSE) {
            g_currentMenu = MENU_NONE;
        }
        else if (g_currentMenu == MENU_SETTINGS && g_previousMenu == MENU_PAUSE) {
            g_currentMenu = MENU_PAUSE;
        }
        else {
            g_currentMenu = MENU_MAIN;
        }
    }

    if (IsKeyPressed(KEY_F)) {
        ToggleBorderlessWindowed();
        systemSettings.video.fullscreen = !systemSettings.video.fullscreen;
    }

    switch (g_currentMenu) {
        case MENU_MAIN:        UpdateMainMenu(); break;
        case MENU_PLAY_CHOICE: UpdatePlayChoiceMenu(); break;
        case MENU_PAUSE:       UpdatePauseMenu(); break;
        case MENU_SETTINGS:    UpdateSettingsMenu(); break;
        case MENU_LEADERBOARD: UpdateLeaderboardMenu(); break;
        default: break;
    }
}

void DrawMenu(void) {
    switch (g_currentMenu) {
        case MENU_MAIN:        DrawMainMenu(); break;
        case MENU_PLAY_CHOICE: DrawPlayChoiceMenu(); break;
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

    int bw = 200;
    int bh = 50;
    Rectangle btnPlay = { sw/2 - bw/2, sh/2 - 100, bw, bh };
    Rectangle btnLB   = { sw/2 - bw/2, sh/2 - 30,  bw, bh };
    Rectangle btnSet  = { sw/2 - bw/2, sh/2 + 40,  bw, bh };
    Rectangle btnExit = { sw/2 - bw/2, sh/2 + 110, bw, bh };

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        if (CheckCollisionPointRec(m, btnPlay)) g_currentMenu = MENU_PLAY_CHOICE;
        if (CheckCollisionPointRec(m, btnLB))   g_currentMenu = MENU_LEADERBOARD;
        if (CheckCollisionPointRec(m, btnSet))  { g_previousMenu = g_currentMenu; g_currentMenu = MENU_SETTINGS; }
        if (CheckCollisionPointRec(m, btnExit)) CloseWindow();
    }
}

void UpdatePlayChoiceMenu(void) {
    Vector2 m = GetMousePosition();
    int sw = GetScreenWidth();
    int sh = GetScreenHeight();

    int bw = 200;
    int bh = 50;
    Rectangle btnSolo  = { sw/2 - bw/2, sh/2 - 100, bw, bh };
    Rectangle btnMulti = { sw/2 - bw/2, sh/2 - 30,  bw, bh };
    Rectangle btnHost  = { sw/2 - bw/2, sh/2 + 40,  bw, bh };
    Rectangle btnBack  = { sw/2 - bw/2, sh/2 + 110, bw, bh };

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        if (CheckCollisionPointRec(m, btnSolo)) {
            // Solo mode logic can go here (or just enter gameplay state locally)
            lobby_game.currentState = GAME_STATE_GAMEPLAY;
            g_currentMenu = MENU_NONE;
        }
        if (CheckCollisionPointRec(m, btnMulti)) {
            lobby_game.currentState = GAME_STATE_CONNECTION;
            g_currentMenu = MENU_NONE; 
        }
        if (CheckCollisionPointRec(m, btnHost)) {
            extern void spawn_server(void);
            spawn_server();
        }
        if (CheckCollisionPointRec(m, btnBack)) g_currentMenu = MENU_MAIN;
    }
}

void DrawPlayChoiceMenu(void) {
    int sw = GetScreenWidth();
    int sh = GetScreenHeight();
    DrawRectangleGradientV(0, 0, sw, sh, DARKBLUE, BLACK);
    DrawText("CHOOSE MODE", sw/2 - MeasureText("CHOOSE MODE", 40)/2, sh/4, 40, GOLD);

    char* labels[] = {"SOLO MODE", "JOIN MULTI", "HOST SERVER", "BACK"};
    int bw = 200;
    int bh = 50;
    for (int i=0; i<4; i++) {
        Rectangle r = { sw/2 - bw/2, sh/2 - 100 + i*70, bw, bh };
        bool hover = CheckCollisionPointRec(GetMousePosition(), r);
        DrawRectangleRounded(r, 0.2f, 10, hover ? LIGHTGRAY : GRAY);
        DrawText(labels[i], r.x + bw/2 - MeasureText(labels[i], 20)/2, r.y + 15, 20, hover ? BLACK : WHITE);
    }
}

void DrawMainMenu(void) {
    int sw = GetScreenWidth();
    int sh = GetScreenHeight();
    DrawRectangleGradientV(0, 0, sw, sh, DARKBLUE, BLACK);

    DrawText("MULTI MINI-GAMES", sw/2 - MeasureText("MULTI MINI-GAMES", 50)/2, sh/4, 50, GOLD);

    char* labels[] = {"PLAY", "LEADERBOARD", "SETTINGS", "EXIT"};
    int bw = 200;
    int bh = 50;
    for (int i=0; i<4; i++) {
        Rectangle r = { sw/2 - bw/2, sh/2 - 100 + i*70, bw, bh };
        bool hover = CheckCollisionPointRec(GetMousePosition(), r);
        DrawRectangleRounded(r, 0.2f, 10, hover ? LIGHTGRAY : GRAY);
        DrawText(labels[i], r.x + bw/2 - MeasureText(labels[i], 20)/2, r.y + 15, 20, hover ? BLACK : WHITE);
    }
}

void UpdatePauseMenu(void) {
    Vector2 m = GetMousePosition();
    int sw = GetScreenWidth();
    int sh = GetScreenHeight();

    int bw = 200;
    int bh = 50;
    Rectangle btnResume = { sw/2 - bw/2, sh/2 - 100, bw, bh };
    Rectangle btnSet    = { sw/2 - bw/2, sh/2 - 30,  bw, bh };
    Rectangle btnAbandon= { sw/2 - bw/2, sh/2 + 40,  bw, bh };
    Rectangle btnQuit   = { sw/2 - bw/2, sh/2 + 110, bw, bh };

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        if (CheckCollisionPointRec(m, btnResume)) g_currentMenu = MENU_NONE;
        if (CheckCollisionPointRec(m, btnSet))    { g_previousMenu = g_currentMenu; g_currentMenu = MENU_SETTINGS; }
        if (CheckCollisionPointRec(m, btnAbandon)) {
            extern LobbyGame_St lobby_game;
            if (lobby_game.currentState == GAME_STATE_CONNECTION || lobby_game.currentState == GAME_STATE_ROOM_LIST) {
                g_currentMenu = MENU_MAIN;
            } else {
                extern void switch_minigame(u8 game_id);
                switch_minigame(0);
                lobby_game.currentState = GAME_STATE_CONNECTION; // Back to discovery
                g_currentMenu = MENU_NONE;
            }
        }
        if (CheckCollisionPointRec(m, btnQuit)) CloseWindow();
    }
}

void DrawPauseMenu(void) {
    int sw = GetScreenWidth();
    int sh = GetScreenHeight();
    DrawRectangle(0, 0, sw, sh, Fade(BLACK, 0.6f));
    DrawRectangleRounded((Rectangle){sw/2 - 150, sh/2 - 150, 300, 370}, 0.1f, 10, DARKGRAY);
    DrawText("PAUSE", sw/2 - MeasureText("PAUSE", 30)/2, sh/2 - 130, 30, WHITE);

    char* labels[] = {"RESUME", "SETTINGS", "ABANDON", "QUIT GAME"};
    int bw = 200;
    int bh = 50;
    for (int i=0; i<4; i++) {
        Rectangle r = { sw/2 - bw/2, sh/2 - 100 + i*70, bw, bh };
        bool hover = CheckCollisionPointRec(GetMousePosition(), r);
        DrawRectangleRounded(r, 0.2f, 10, hover ? LIGHTGRAY : GRAY);
        DrawText(labels[i], r.x + bw/2 - MeasureText(labels[i], 20)/2, r.y + 15, 20, (i >= 2) ? MAROON : (hover ? BLACK : WHITE));
    }
}

void UpdateSettingsMenu(void) {
    if (IsKeyPressed(KEY_M)) systemSettings.audio.musicVolume = (systemSettings.audio.musicVolume + 0.1f);
    if (systemSettings.audio.musicVolume > 1.0f) systemSettings.audio.musicVolume = 0.0f;

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        int sw = GetScreenWidth();
        int sh = GetScreenHeight();
        Rectangle btnBack = { sw/2 - 100, sh - 120, 200, 50 };
        if (CheckCollisionPointRec(GetMousePosition(), btnBack)) g_currentMenu = g_previousMenu;
    }
}

void DrawSettingsMenu(void) {
    int sw = GetScreenWidth();
    int sh = GetScreenHeight();
    DrawRectangle(0, 0, sw, sh, Fade(BLACK, 0.8f));
    DrawText("SETTINGS", sw/2 - 100, 100, 40, WHITE);
    DrawText(TextFormat("Music Volume: %.1f  (M)", systemSettings.audio.musicVolume), 100, 200, 25, LIGHTGRAY);
    DrawText(TextFormat("Windowed Fullscreen: %s (F)", systemSettings.video.fullscreen ? "ON" : "OFF"), 100, 250, 25, LIGHTGRAY);
    
    Rectangle btnBack = { sw/2 - 100, sh - 120, 200, 50 };
    bool hover = CheckCollisionPointRec(GetMousePosition(), btnBack);
    DrawRectangleRounded(btnBack, 0.2f, 10, hover ? LIGHTGRAY : GRAY);
    DrawText("BACK", btnBack.x + 100 - MeasureText("BACK", 20)/2, btnBack.y + 15, 20, hover ? BLACK : WHITE);

    DrawText("ESC pour retour", 100, sh - 50, 20, GRAY);
}

void UpdateLeaderboardMenu(void) {
    if (IsKeyPressed(KEY_ESCAPE)) g_currentMenu = MENU_MAIN;
    
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        int sw = GetScreenWidth();
        int sh = GetScreenHeight();
        Rectangle btnBack = { sw/2 - 100, sh - 120, 200, 50 };
        if (CheckCollisionPointRec(GetMousePosition(), btnBack)) g_currentMenu = MENU_MAIN;
    }
}

void DrawLeaderboardMenu(void) {
    int sw = GetScreenWidth();
    int sh = GetScreenHeight();
    DrawRectangle(0, 0, sw, sh, Fade(BLACK, 0.9f));
    DrawText("TOP 10 - KING FOR FOUR", sw/2 - 200, 80, 40, GOLD);
    
    Leaderboard_St lb = LoadLeaderboard(1);
    int startY = 160;
    for (int i=0; i<10 && i<lb.count; i++) {
        DrawText(TextFormat("%d. %s", i+1, lb.entries[i].name), 150, startY + i*35, 25, (i == 0) ? GOLD : WHITE);
        DrawText(TextFormat("%d pts", lb.entries[i].score), sw - 350, startY + i*35, 25, (i == 0) ? GOLD : WHITE);
    }

    Rectangle btnBack = { sw/2 - 100, sh - 120, 200, 50 };
    bool hover = CheckCollisionPointRec(GetMousePosition(), btnBack);
    DrawRectangleRounded(btnBack, 0.2f, 10, hover ? LIGHTGRAY : GRAY);
    DrawText("BACK", btnBack.x + 100 - MeasureText("BACK", 20)/2, btnBack.y + 15, 20, hover ? BLACK : WHITE);

    DrawText("ESC pour retour", sw/2 - 100, sh - 50, 20, GRAY);
}
