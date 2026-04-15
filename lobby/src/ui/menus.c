/**
    @file ui/menus.c
    @author i-Charlys (CAILLON Charles)
    @author Fshimi-Hawlk
    @date 2026-04-13
    @date 2026-04-13
    @brief Menu system for the lobby (Main, Play Choice, Pause, Settings, Leaderboard).

    Contributors:
        - i-Charlys:
            - Original menu logic
        - Fshimi-Hawlk:
            - Full widget migration, lobby_fonts consistency, correct title positioning,
              CSC compliance (lobby_ prefix, s32/u32, exact call formatting)

    @note All UI now uses the shared TextButton_St widget system.
          All text uses lobby_fonts[]. Titles are positioned higher above buttons.
*/

#include "ui/menus.h"
#include "utils/globals.h"

#include "leaderboard.h"

#include "sharedWidgets/button.h"
#include "sharedWidgets/types.h"

MenuType_Et lobby_currentMenu = MENU_MAIN;
MenuType_Et lobby_previousMenu = MENU_MAIN;

extern LobbyGame_St lobby_game;

/* =====================================================================
   Initialization
   ===================================================================== */

void lobby_initMenus(void) {
    lobby_currentMenu = MENU_MAIN;
    lobby_previousMenu = MENU_MAIN;
}

/* =====================================================================
   Update
   ===================================================================== */

void lobby_updateMenu(void) {
    if (IsKeyPressed(KEY_ESCAPE)) {
        if (lobby_currentMenu == MENU_NONE) {
            lobby_previousMenu = MENU_NONE;
            lobby_currentMenu = MENU_PAUSE;
        } else if (lobby_currentMenu == MENU_PAUSE) {
            lobby_currentMenu = MENU_NONE;
        } else if (lobby_currentMenu == MENU_SETTINGS && lobby_previousMenu == MENU_PAUSE) {
            lobby_currentMenu = MENU_PAUSE;
        } else {
            lobby_currentMenu = MENU_MAIN;
        }
    }

    if (IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_F)) {
        ToggleBorderlessWindowed();
        systemSettings.video.fullscreen = !systemSettings.video.fullscreen;
    }

    switch (lobby_currentMenu) {
        case MENU_MAIN:        lobby_updateMainMenu(); break;
        case MENU_PLAY_CHOICE: lobby_updatePlayChoiceMenu(); break;
        case MENU_PAUSE:       lobby_updatePauseMenu(); break;
        case MENU_SETTINGS:    lobby_updateSettingsMenu(); break;
        case MENU_LEADERBOARD: lobby_updateLeaderboardMenu(); break;
        default: break;
    }
}

/* =====================================================================
   Draw
   ===================================================================== */

void lobby_drawMenu(void) {
    switch (lobby_currentMenu) {
        case MENU_MAIN:        lobby_drawMainMenu(); break;
        case MENU_PLAY_CHOICE: lobby_drawPlayChoiceMenu(); break;
        case MENU_PAUSE:       lobby_drawPauseMenu(); break;
        case MENU_SETTINGS:    lobby_drawSettingsMenu(); break;
        case MENU_LEADERBOARD: lobby_drawLeaderboardMenu(); break;
        default: break;
    }
}

/* =====================================================================
   Main Menu
   ===================================================================== */

void lobby_updateMainMenu(void) {
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
        if (CheckCollisionPointRec(m, btnPlay)) lobby_currentMenu = MENU_PLAY_CHOICE;
        if (CheckCollisionPointRec(m, btnLB))   lobby_currentMenu = MENU_LEADERBOARD;
        if (CheckCollisionPointRec(m, btnSet))  { lobby_previousMenu = lobby_currentMenu; lobby_currentMenu = MENU_SETTINGS; }
        if (CheckCollisionPointRec(m, btnExit)) g_shouldExit = true;
    }
}

void lobby_drawMainMenu(void) {
    s32 sw = GetScreenWidth();
    s32 sh = GetScreenHeight();
    DrawRectangleGradientV(0, 0, sw, sh, DARKBLUE, BLACK);

    /* Title – positioned higher */
    DrawTextEx(
        lobby_fonts[FONT48], "MULTI MINI-GAMES",
        (Vector2){sw/2.0f - MeasureTextEx(lobby_fonts[FONT48], "MULTI MINI-GAMES", 48, 2).x / 2.0f, sh/4.0f - 60.0f},
        48, 2, GOLD
    );

    char* labels[] = {"PLAY", "LEADERBOARD", "SETTINGS", "EXIT"};
    s32 bw = 200;
    s32 bh = 50;

    for (s32 i = 0; i < 4; i++) {
        Rectangle r = { sw/2.0f - bw/2.0f, sh/2.0f - 100 + i*70, bw, bh };
        bool hover = CheckCollisionPointRec(GetMousePosition(), r);

        TextButton_St btn = {
            .bounds    = r,
            .state     = hover ? WIDGET_STATE_HOVER : WIDGET_STATE_NORMAL,
            .baseColor = hover ? LIGHTGRAY : GRAY,
            .roundness = 0.2f,
            .textColor = WHITE,
            .text      = labels[i]
        };

        textButtonDraw(&btn, lobby_fonts[FONT24], 24.0f);
    }
}

/* =====================================================================
   Play Choice Menu
   ===================================================================== */

void lobby_updatePlayChoiceMenu(void) {
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
            lobby_game.currentState = GAME_STATE_GAMEPLAY;
            lobby_currentMenu = MENU_NONE;
        }
        if (CheckCollisionPointRec(m, btnMulti)) {
            lobby_game.currentState = GAME_STATE_CONNECTION;
            lobby_currentMenu = MENU_NONE;
        }
        if (CheckCollisionPointRec(m, btnHost)) {
            extern void spawn_server(void);
            spawn_server();
            lobby_game.currentState = GAME_STATE_CONNECTION;
            lobby_currentMenu = MENU_NONE;
        }
        if (CheckCollisionPointRec(m, btnBack)) lobby_currentMenu = MENU_MAIN;
    }
}

void lobby_drawPlayChoiceMenu(void) {
    s32 sw = GetScreenWidth();
    s32 sh = GetScreenHeight();
    DrawRectangleGradientV(0, 0, sw, sh, DARKBLUE, BLACK);

    /* Title – positioned higher */
    DrawTextEx(
        lobby_fonts[FONT48], "CHOOSE MODE",
        (Vector2){sw/2.0f - MeasureTextEx(lobby_fonts[FONT48], "CHOOSE MODE", 48, 2).x / 2.0f, sh/4.0f - 60.0f},
        48, 2, GOLD
    );

    char* labels[] = {"SOLO MODE", "JOIN MULTI", "HOST SERVER", "BACK"};
    s32 bw = 200;
    s32 bh = 50;

    for (s32 i = 0; i < 4; i++) {
        Rectangle r = { sw/2.0f - bw/2.0f, sh/2.0f - 100 + i*70, bw, bh };
        bool hover = CheckCollisionPointRec(GetMousePosition(), r);

        TextButton_St btn = {
            .bounds    = r,
            .state     = hover ? WIDGET_STATE_HOVER : WIDGET_STATE_NORMAL,
            .baseColor = hover ? LIGHTGRAY : GRAY,
            .roundness = 0.2f,
            .textColor = WHITE,
            .text      = labels[i]
        };

        textButtonDraw(&btn, lobby_fonts[FONT24], 24.0f);
    }
}

/* =====================================================================
   Pause Menu
   ===================================================================== */

void lobby_updatePauseMenu(void) {
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
        if (CheckCollisionPointRec(m, btnResume)) lobby_currentMenu = MENU_NONE;
        if (CheckCollisionPointRec(m, btnSet))    { lobby_previousMenu = lobby_currentMenu; lobby_currentMenu = MENU_SETTINGS; }
        if (CheckCollisionPointRec(m, btnAbandon)) {
            extern LobbyGame_St lobby_game;
            if (lobby_game.currentState == GAME_STATE_INGAME) {
                extern void switchMinigame(u8 game_id);
                switchMinigame(MINI_GAME_ID_LOBBY);
                lobby_currentMenu = MENU_NONE;
            } else if (lobby_game.currentState == GAME_STATE_GAMEPLAY || lobby_game.currentState == GAME_STATE_ROOM_LIST) {
                lobby_game.currentState = GAME_STATE_CONNECTION;
                lobby_currentMenu = MENU_NONE;
            } else {
                lobby_currentMenu = MENU_MAIN;
            }
        }
        if (CheckCollisionPointRec(m, btnQuit)) g_shouldExit = true;
    }
}

void lobby_drawPauseMenu(void) {
    s32 sw = GetScreenWidth();
    s32 sh = GetScreenHeight();

    DrawRectangle(0, 0, sw, sh, Fade(BLACK, 0.6f));
    DrawRectangleRounded((Rectangle){sw/2.0f - 150, sh/2.0f - 150, 300, 370}, 0.1f, 10, DARKGRAY);

    DrawTextEx(
        lobby_fonts[FONT32], "PAUSE",
        (Vector2){sw/2.0f - MeasureTextEx(lobby_fonts[FONT32], "PAUSE", 32, 0).x / 2.0f, sh/2.0f - 130},
        32, 0, WHITE
    );

    char* labels[] = {"RESUME", "SETTINGS", "ABANDON", "QUIT GAME"};
    s32 bw = 200;
    s32 bh = 50;

    for (s32 i = 0; i < 4; i++) {
        Rectangle r = { sw/2.0f - bw/2.0f, sh/2.0f - 100 + i*70, bw, bh };
        bool hover = CheckCollisionPointRec(GetMousePosition(), r);

        TextButton_St btn = {
            .bounds    = r,
            .state     = hover ? WIDGET_STATE_HOVER : WIDGET_STATE_NORMAL,
            .baseColor = (i >= 2) ? MAROON : (hover ? LIGHTGRAY : GRAY),
            .roundness = 0.2f,
            .textColor = WHITE,
            .text      = labels[i]
        };

        textButtonDraw(&btn, lobby_fonts[FONT24], 24.0f);
    }
}

/* =====================================================================
   Settings Menu
   ===================================================================== */

void lobby_updateSettingsMenu(void) {
    if (IsKeyPressed(KEY_M) || IsKeyPressed(KEY_SEMICOLON)) {
        systemSettings.audio.musicVolume = (systemSettings.audio.musicVolume + 0.1f);
        if (systemSettings.audio.musicVolume > 1.0f) systemSettings.audio.musicVolume = 0.0f;
    }

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        s32 sw = GetScreenWidth();
        s32 sh = GetScreenHeight();
        Rectangle btnBack = { sw/2.0f - 100, sh - 120, 200, 50 };
        if (CheckCollisionPointRec(GetMousePosition(), btnBack)) {
            lobby_currentMenu = lobby_previousMenu;
        }
    }
}

void lobby_drawSettingsMenu(void) {
    s32 sw = GetScreenWidth();
    s32 sh = GetScreenHeight();

    DrawRectangle(0, 0, sw, sh, Fade(BLACK, 0.8f));

    DrawTextEx(
        lobby_fonts[FONT48], "SETTINGS",
        (Vector2){sw/2.0f - MeasureTextEx(lobby_fonts[FONT48], "SETTINGS", 40, 0).x / 2.0f, 100},
        40, 0, WHITE
    );

    DrawTextEx(
        lobby_fonts[FONT24],
        TextFormat("Music Volume: %.1f  (M)", systemSettings.audio.musicVolume),
        (Vector2){100, 200}, 24, 0, LIGHTGRAY
    );

    DrawTextEx(
        lobby_fonts[FONT24],
        TextFormat("Windowed Fullscreen: %s (Ctrl+F)", systemSettings.video.fullscreen ? "ON" : "OFF"),
        (Vector2){100, 250}, 24, 0, LIGHTGRAY
    );

    Rectangle btnBack = { sw/2.0f - 100, sh - 120, 200, 50 };
    bool hover = CheckCollisionPointRec(GetMousePosition(), btnBack);

    TextButton_St btn = {
        .bounds    = btnBack,
        .state     = hover ? WIDGET_STATE_HOVER : WIDGET_STATE_NORMAL,
        .baseColor = hover ? LIGHTGRAY : GRAY,
        .roundness = 0.2f,
        .text      = "BACK"
    };
    textButtonDraw(&btn, lobby_fonts[FONT24], 24.0f);

    DrawTextEx(lobby_fonts[FONT24], "ESC pour retour", (Vector2){100, sh - 50}, 20, 0, GRAY);
}

/* =====================================================================
   Leaderboard Menu
   ===================================================================== */

void lobby_updateLeaderboardMenu(void) {
    if (IsKeyPressed(KEY_ESCAPE)) lobby_currentMenu = MENU_MAIN;

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        s32 sw = GetScreenWidth();
        s32 sh = GetScreenHeight();
        Rectangle btnBack = { sw/2.0f - 100, sh - 120, 200, 50 };
        if (CheckCollisionPointRec(GetMousePosition(), btnBack)) lobby_currentMenu = MENU_MAIN;
    }
}

void lobby_drawLeaderboardMenu(void) {
    s32 sw = GetScreenWidth();
    s32 sh = GetScreenHeight();

    DrawRectangle(0, 0, sw, sh, Fade(BLACK, 0.9f));

    DrawTextEx(
        lobby_fonts[FONT48], "TOP 10 - KING FOR FOUR",
        (Vector2){sw/2.0f - MeasureTextEx(lobby_fonts[FONT48], "TOP 10 - KING FOR FOUR", 40, 0).x / 2.0f, 80},
        40, 0, GOLD
    );

    Leaderboard_St lb = leaderboard_load(1);
    s32 startY = 160;

    for (u32 i = 0; i < 10 && i < lb.count; i++) {
        DrawTextEx(
            lobby_fonts[FONT24],
            TextFormat("%d. %s", i+1, lb.items[i].name),
            (Vector2){150, startY + i*35}, 24, 0, (i == 0) ? GOLD : WHITE
        );

        DrawTextEx(
            lobby_fonts[FONT24],
            TextFormat("%d pts", lb.items[i].score),
            (Vector2){sw - 350, startY + i*35}, 24, 0, (i == 0) ? GOLD : WHITE
        );
    }

    Rectangle btnBack = { sw/2.0f - 100, sh - 120, 200, 50 };
    bool hover = CheckCollisionPointRec(GetMousePosition(), btnBack);

    TextButton_St btn = {
        .bounds    = btnBack,
        .state     = hover ? WIDGET_STATE_HOVER : WIDGET_STATE_NORMAL,
        .baseColor = hover ? LIGHTGRAY : GRAY,
        .roundness = 0.2f,
        .text      = "BACK"
    };
    textButtonDraw(&btn, lobby_fonts[FONT24], 24.0f);

    DrawTextEx(lobby_fonts[FONT24], "ESC pour retour", (Vector2){sw/2.0f - 100, sh - 50}, 20, 0, GRAY);
}