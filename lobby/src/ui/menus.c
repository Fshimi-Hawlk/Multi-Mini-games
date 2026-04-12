/**
    @file ui/connectionScreen.c
    @author i-Charlys (CAILLON Charles)
    @author Fshimi-Hawlk
    @date 2026-04-08
    @date 2026-04-11
    @brief Modern connection screen using the enhanced reusable widget system.
*/

#include "utils/userTypes.h"

#include "ui/menus.h"
#include "leaderboard.h"

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

    if (IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_F)) {
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
    s32 sw = GetScreenWidth();
    s32 sh = GetScreenHeight();

    s32 bw = 200;
    s32 bh = 50;
    Rectangle btnPlay = { sw/2.0f - bw/2.0f, sh/2.0f - 100, bw, bh };
    Rectangle btnLB   = { sw/2.0f - bw/2.0f, sh/2.0f - 30,  bw, bh };
    Rectangle btnSet  = { sw/2.0f - bw/2.0f, sh/2.0f + 40,  bw, bh };
    Rectangle btnExit = { sw/2.0f - bw/2.0f, sh/2.0f + 110, bw, bh };

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        if (CheckCollisionPointRec(m, btnPlay)) g_currentMenu = MENU_PLAY_CHOICE;
        if (CheckCollisionPointRec(m, btnLB))   g_currentMenu = MENU_LEADERBOARD;
        if (CheckCollisionPointRec(m, btnSet))  { g_previousMenu = g_currentMenu; g_currentMenu = MENU_SETTINGS; }
        if (CheckCollisionPointRec(m, btnExit)) CloseWindow();
    }
}

void UpdatePlayChoiceMenu(void) {
    Vector2 m = GetMousePosition();
    s32 sw = GetScreenWidth();
    s32 sh = GetScreenHeight();

    s32 bw = 200;
    s32 bh = 50;
    Rectangle btnSolo  = { sw/2.0f - bw/2.0f, sh/2.0f - 100, bw, bh };
    Rectangle btnMulti = { sw/2.0f - bw/2.0f, sh/2.0f - 30,  bw, bh };
    Rectangle btnHost  = { sw/2.0f - bw/2.0f, sh/2.0f + 40,  bw, bh };
    Rectangle btnBack  = { sw/2.0f - bw/2.0f, sh/2.0f + 110, bw, bh };

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
    s32 sw = GetScreenWidth();
    s32 sh = GetScreenHeight();
    DrawRectangleGradientV(0, 0, sw, sh, DARKBLUE, BLACK);
    DrawText("CHOOSE MODE", sw/2.0f - MeasureText("CHOOSE MODE", 40)/2.0f, sh/4, 40, GOLD);

    char* labels[] = {"SOLO MODE", "JOIN MULTI", "HOST SERVER", "BACK"};
    s32 bw = 200;
    s32 bh = 50;
    for (s32 i=0; i<4; i++) {
        Rectangle r = { sw/2.0f - bw/2.0f, sh/2.0f - 100 + i*70, bw, bh };
        bool hover = CheckCollisionPointRec(GetMousePosition(), r);
        DrawRectangleRounded(r, 0.2f, 10, hover ? LIGHTGRAY : GRAY);
        DrawText(labels[i], r.x + bw/2.0f - MeasureText(labels[i], 20)/2.0f, r.y + 15, 20, hover ? BLACK : WHITE);
    }
}

void DrawMainMenu(void) {
    s32 sw = GetScreenWidth();
    s32 sh = GetScreenHeight();
    DrawRectangleGradientV(0, 0, sw, sh, DARKBLUE, BLACK);

    DrawText("MULTI MINI-GAMES", sw/2.0f - MeasureText("MULTI MINI-GAMES", 50)/2.0f, sh/4, 50, GOLD);

    char* labels[] = {"PLAY", "LEADERBOARD", "SETTINGS", "EXIT"};
    s32 bw = 200;
    s32 bh = 50;
    for (s32 i=0; i<4; i++) {
        Rectangle r = { sw/2.0f - bw/2.0f, sh/2.0f - 100 + i*70, bw, bh };
        bool hover = CheckCollisionPointRec(GetMousePosition(), r);
        DrawRectangleRounded(r, 0.2f, 10, hover ? LIGHTGRAY : GRAY);
        DrawText(labels[i], r.x + bw/2.0f - MeasureText(labels[i], 20)/2.0f, r.y + 15, 20, hover ? BLACK : WHITE);
    }
}

void UpdatePauseMenu(void) {
    Vector2 m = GetMousePosition();
    s32 sw = GetScreenWidth();
    s32 sh = GetScreenHeight();

    s32 bw = 200;
    s32 bh = 50;
    Rectangle btnResume = { sw/2.0f - bw/2.0f, sh/2.0f - 100, bw, bh };
    Rectangle btnSet    = { sw/2.0f - bw/2.0f, sh/2.0f - 30,  bw, bh };
    Rectangle btnAbandon= { sw/2.0f - bw/2.0f, sh/2.0f + 40,  bw, bh };
    Rectangle btnQuit   = { sw/2.0f - bw/2.0f, sh/2.0f + 110, bw, bh };

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        if (CheckCollisionPointRec(m, btnResume)) g_currentMenu = MENU_NONE;
        if (CheckCollisionPointRec(m, btnSet))    { g_previousMenu = g_currentMenu; g_currentMenu = MENU_SETTINGS; }
        if (CheckCollisionPointRec(m, btnAbandon)) {
            extern LobbyGame_St lobby_game;
            if (lobby_game.currentState == GAME_STATE_INGAME) {
                // Return to lobby from minigame
                extern void switch_minigame(u8 game_id);
                switch_minigame(0);
                g_currentMenu = MENU_NONE;
            } else if (lobby_game.currentState == GAME_STATE_GAMEPLAY || lobby_game.currentState == GAME_STATE_ROOM_LIST) {
                // Return to server selection from lobby
                lobby_game.currentState = GAME_STATE_CONNECTION;
                g_currentMenu = MENU_NONE;
            } else {
                g_currentMenu = MENU_MAIN;
            }
        }
        if (CheckCollisionPointRec(m, btnQuit)) CloseWindow();
    }
}

void DrawPauseMenu(void) {
    s32 sw = GetScreenWidth();
    s32 sh = GetScreenHeight();
    DrawRectangle(0, 0, sw, sh, Fade(BLACK, 0.6f));
    DrawRectangleRounded((Rectangle){sw/2.0f - 150, sh/2.0f - 150, 300, 370}, 0.1f, 10, DARKGRAY);
    DrawText("PAUSE", sw/2.0f - MeasureText("PAUSE", 30)/2.0f, sh/2.0f - 130, 30, WHITE);

    char* labels[] = {"RESUME", "SETTINGS", "ABANDON", "QUIT GAME"};
    s32 bw = 200;
    s32 bh = 50;
    for (s32 i=0; i<4; i++) {
        Rectangle r = { sw/2.0f - bw/2.0f, sh/2.0f - 100 + i*70, bw, bh };
        bool hover = CheckCollisionPointRec(GetMousePosition(), r);
        DrawRectangleRounded(r, 0.2f, 10, hover ? LIGHTGRAY : GRAY);
        DrawText(labels[i], r.x + bw/2.0f - MeasureText(labels[i], 20)/2.0f, r.y + 15, 20, (i >= 2) ? MAROON : (hover ? BLACK : WHITE));
    }
}

void UpdateSettingsMenu(void) {
    if (IsKeyPressed(KEY_M) || IsKeyPressed(KEY_SEMICOLON)) systemSettings.audio.musicVolume = (systemSettings.audio.musicVolume + 0.1f);
    if (systemSettings.audio.musicVolume > 1.0f) systemSettings.audio.musicVolume = 0.0f;

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        s32 sw = GetScreenWidth();
        s32 sh = GetScreenHeight();
        Rectangle btnBack = { sw/2.0f - 100, sh - 120, 200, 50 };
        if (CheckCollisionPointRec(GetMousePosition(), btnBack)) g_currentMenu = g_previousMenu;
    }
}

void DrawSettingsMenu(void) {
    s32 sw = GetScreenWidth();
    s32 sh = GetScreenHeight();
    DrawRectangle(0, 0, sw, sh, Fade(BLACK, 0.8f));
    DrawText("SETTINGS", sw/2.0f - 100, 100, 40, WHITE);
    DrawText(TextFormat("Music Volume: %.1f  (M)", systemSettings.audio.musicVolume), 100, 200, 25, LIGHTGRAY);
    DrawText(TextFormat("Windowed Fullscreen: %s (Ctrl+F)", systemSettings.video.fullscreen ? "ON" : "OFF"), 100, 250, 25, LIGHTGRAY);
    
    Rectangle btnBack = { sw/2.0f - 100, sh - 120, 200, 50 };
    bool hover = CheckCollisionPointRec(GetMousePosition(), btnBack);
    DrawRectangleRounded(btnBack, 0.2f, 10, hover ? LIGHTGRAY : GRAY);
    DrawText("BACK", btnBack.x + 100 - MeasureText("BACK", 20)/2.0f, btnBack.y + 15, 20, hover ? BLACK : WHITE);

    DrawText("ESC pour retour", 100, sh - 50, 20, GRAY);
}

void UpdateLeaderboardMenu(void) {
    if (IsKeyPressed(KEY_ESCAPE)) g_currentMenu = MENU_MAIN;
    
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        s32 sw = GetScreenWidth();
        s32 sh = GetScreenHeight();
        Rectangle btnBack = { sw/2.0f - 100, sh - 120, 200, 50 };
        if (CheckCollisionPointRec(GetMousePosition(), btnBack)) g_currentMenu = MENU_MAIN;
    }
}

void DrawLeaderboardMenu(void) {
    s32 sw = GetScreenWidth();
    s32 sh = GetScreenHeight();
    DrawRectangle(0, 0, sw, sh, Fade(BLACK, 0.9f));
    DrawText("TOP 10 - KING FOR FOUR", sw/2.0f - 200, 80, 40, GOLD);
    
    Leaderboard_St lb = leaderboard_load(1);
    s32 startY = 160;
    for (u32 i = 0; i < 10 && i < lb.count; i++) {
        DrawText(TextFormat("%d. %s", i+1, lb.items[i].name), 150, startY + i*35, 25, (i == 0) ? GOLD : WHITE);
        DrawText(TextFormat("%d pts", lb.items[i].score), sw - 350, startY + i*35, 25, (i == 0) ? GOLD : WHITE);
    }

    Rectangle btnBack = { sw/2.0f - 100, sh - 120, 200, 50 };
    bool hover = CheckCollisionPointRec(GetMousePosition(), btnBack);
    DrawRectangleRounded(btnBack, 0.2f, 10, hover ? LIGHTGRAY : GRAY);
    DrawText("BACK", btnBack.x + 100 - MeasureText("BACK", 20)/2.0f, btnBack.y + 15, 20, hover ? BLACK : WHITE);

    DrawText("ESC pour retour", sw/2.0f - 100, sh - 50, 20, GRAY);
}
