/**
    @file ui/app.c
    @author Fshimi-Hawlk
    @date 2026-03-02
    @date 2026-03-05
    @brief One clear sentence that tells what this file is actually for.

    Contributors:
        - Fshimi-Hawlk:
            - Provided documentation start-up
        - <Name>:
            - What you added / changed / fixed (keep it short)

    If the file needs more context than fits in @brief, write 2-5 lines here.
    @note Put warnings, important limitations, "we know it's ugly but...", or future plans here

    // Try to align the `for` for better readability
    // Try to keep the same order of the includes
    Use @see `path/to/related/file.h` when this file depends heavily on another one.
*/

#include "ui/app.h"
#include "utils/globals.h"

void bingo_drawUI(const BingoGame_St* game) {
    // Timer during grace
    if (SHOW_DELAY <= game->currentCall.timer && game->currentCall.timer <= (SHOW_DELAY + GRACE_TIME)) {
        f32 fontSize = 32;
        char buf[16];
        sprintf(buf, "%.2f", GRACE_TIME * ((SHOW_DELAY + GRACE_TIME) - game->currentCall.timer));
        f32Vector2 textSize = MeasureTextEx(fonts[FONT48], buf, fontSize, 0);
        f32Vector2 textPos = {
            .x = game->layout.windowCenter.x - textSize.x / 2.0f,
            .y = game->layout.cardRect.y + game->layout.cardRect.height + fontSize * 1.5f,
        };

        DrawTextEx(fonts[FONT48], buf, textPos, fontSize, 0, BLACK);
    }

    // Fading ball call
    if (game->currentCall.timer <= SHOW_DELAY) {
        f32 fontSize = 48;
        
        f32Vector2 textSize = MeasureTextEx(fonts[FONT48], game->currentCall.displayedText, 0, fontSize);
        f32Vector2 textPos = {
            .x = game->layout.windowCenter.x - textSize.x / 4.0f,
            .y = game->layout.cardRect.y - fontSize * 1.5f,
        };

        DrawTextEx(
            fonts[FONT48],
            game->currentCall.displayedText,
            textPos, fontSize, 0,
            Fade(BLACK, 
                1.0f - (game->currentCall.timer / SHOW_DELAY)
            )
        );
    }
}


static void updatePauseMenu(void);
static void drawPauseOverlay(void);
static void updateConfirm(void);
static void drawConfirm(const char *message);
static void updateSettings(void);
static void drawSettings(void);
static void updateKeybinds(void);
static void drawKeybinds(void);

static Rectangle getMenuItemRect(s32 index, s32 fontSize, s32 baseY);

static const char *pauseMenuTexts[__menuItemCount] = {
    [MENU_RESUME]   = "Resume",
    [MENU_SETTINGS] = "Settings",
    [MENU_RESTART]  = "Restart",
    [MENU_ABANDON]  = "Abandon"
};

static const char *confirmTexts[2] = {"Yes", "No"};

// Placeholder keybind names and current keys (modifiable)
static const char *keybindNames[__keybindActionCount] = {
    [ACTION_FULLSCREEN] = "Fullscreen",
    [ACTION_PAUSE] = "Pause"
};
static s32 currentKeybinds[__keybindActionCount] = {
    [ACTION_FULLSCREEN] = KEY_F,
    [ACTION_PAUSE]      = KEY_ESCAPE
};

//------------------------------------------------------------------------------------
// Module state
//------------------------------------------------------------------------------------
static GameState_Et currentState = STATE_GAMEPLAY;
static MenuItem_Et selectedItem = MENU_RESUME;

static s32 settingsTab = 0;         // 0 = app, 1 = game
static s32 confirmChoice = 0;       // For yes/no screens
static s32 rebindingAction = -1;    // -1 = none, else action index being rebound

static bool gameShouldClose = false;                // Custom flag to force loop exit


static Rectangle getMenuItemRect(s32 index, s32 fontSize, s32 baseY) {
    float textWidth = MeasureTextEx(fonts[fontSize], pauseMenuTexts[index], (float)fontSize, 2).x;
    s32 x = GetScreenWidth()/2.0f - (s32) textWidth/2.0f;
    s32 y = baseY + index * 60;
    return (Rectangle){ (float)x - 20, (float)y - 10, textWidth + 40, (float)fontSize + 20 };
}

static void updatePauseMenu(void) {
    Vector2 mouse = GetMousePosition();

    // Keyboard nav
    if (IsKeyPressed(KEY_DOWN)) {
        selectedItem = (selectedItem + 1) % __menuItemCount;
    }
    if (IsKeyPressed(KEY_UP)) {
        selectedItem = (selectedItem - 1 + __menuItemCount) % __menuItemCount;
    }

    // Mouse selection + click
    for (s32 i = 0; i < __menuItemCount; i++) {
        Rectangle r = getMenuItemRect(i, 50, 240);
        if (CheckCollisionPointRec(mouse, r)) {
            selectedItem = (MenuItem_Et)i;
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                goto activate_item;
            }
        }
    }

    // Keyboard activation
    if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) {
activate_item:
        switch (selectedItem) {
            case MENU_RESUME:
                currentState = STATE_GAMEPLAY;
                break;

            case MENU_SETTINGS:
                currentState = STATE_SETTINGS_APP;
                settingsTab = 0;
                selectedItem = MENU_RESUME;
                break;

            case MENU_RESTART:
                currentState = STATE_RESTART_CONFIRM;
                confirmChoice = 1;
                break;

            case MENU_ABANDON:
                currentState = STATE_ABANDON_CONFIRM;
                confirmChoice = 1;
                break;

            default:
                break;
        }
    }

    if (IsKeyPressed(KEY_ESCAPE)) {
        currentState = STATE_GAMEPLAY;
    }
}

static void drawPauseOverlay(void) {
    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(BLACK, 0.7f));

    const char *title = "PAUSED";
    Vector2 titleSize = MeasureTextEx(fonts[FONT80], title, 80, 3);
    DrawTextEx(fonts[FONT80], title, (Vector2){GetScreenWidth()/2.0f - titleSize.x/2, 120}, 80, 3, WHITE);

    Vector2 mouse = GetMousePosition();
    const s32 baseY = 240;
    for (u32 i = 0; i < __menuItemCount; i++) {
        Rectangle r = getMenuItemRect(i, 50, baseY);
        Color color = (i == selectedItem) ? YELLOW : WHITE;

        if (CheckCollisionPointRec(mouse, r)) {
            DrawRectangleRec(r, Fade(WHITE, 0.15f));
        }

        DrawTextEx(fonts[FONT50], pauseMenuTexts[i], (Vector2){r.x + 20, r.y + 10}, 50, 2, color);
    }

    DrawTextEx(fonts[FONT20], "UP/DOWN or mouse: select   ENTER/SPACE or click: confirm   ESC: resume",
               (Vector2){GetScreenWidth()/2.0f - MeasureTextEx(fonts[FONT20], "...", 20, 1).x/2,
                         GetScreenHeight() - 80}, 20, 1, LIGHTGRAY);
}

//------------------------------------------------------------------------------------
static void updateConfirm(void) {
    if (IsKeyPressed(KEY_LEFT))  confirmChoice = 0;
    if (IsKeyPressed(KEY_RIGHT)) confirmChoice = 1;

    if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) {
        if (confirmChoice == 0) {       // Yes
            if (currentState == STATE_RESTART_CONFIRM) {
                // Restart
            } else if (currentState == STATE_ABANDON_CONFIRM) {
                // Abandon
                gameShouldClose = true;
            }
        } else {                          // No
            currentState = STATE_PAUSED;
            selectedItem = MENU_RESTART;    // or MENU_ABANDON depending
        }
    }

    if (IsKeyPressed(KEY_ESCAPE)) {
        currentState = STATE_PAUSED;
    }
}

static void drawConfirm(const char *message) {
    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(BLACK, 0.8f));

    Vector2 msgSize = MeasureTextEx(fonts[FONT60], message, 60, 2);
    DrawTextEx(fonts[FONT60], message, (Vector2){GetScreenWidth()/2.0f - msgSize.x/2, 180}, 60, 2, WHITE);

    const s32 baseY = 300;
    for (s32 i = 0; i < 2; i++) {
        Vector2 size = MeasureTextEx(fonts[FONT50], confirmTexts[i], 50, 2);
        float x = GetScreenWidth()/2.0f + (i == 0 ? -150 : 80) - size.x/2;
        Rectangle r = {x - 20, baseY - 10, size.x + 40, 70};

        Color color = (i == confirmChoice) ? YELLOW : WHITE;
        DrawTextEx(fonts[FONT50], confirmTexts[i], (Vector2){r.x + 20, r.y + 10}, 50, 2, color);
    }

    DrawTextEx(fonts[FONT20], "LEFT/RIGHT or mouse: select   ENTER/SPACE: confirm   ESC: back",
               (Vector2){GetScreenWidth()/2.0f - MeasureTextEx(fonts[FONT20], "...", 20, 1).x/2,
                         GetScreenHeight() - 80}, 20, 1, LIGHTGRAY);
}

//------------------------------------------------------------------------------------
static void updateSettings(void) {
    if (IsKeyPressed(KEY_ESCAPE)) {
        currentState = STATE_PAUSED;
        selectedItem = MENU_SETTINGS;
        rebindingAction = -1;
    }

    if (currentState == STATE_SETTINGS_KEYBINDS) {
        updateKeybinds();
    }
    else {
        // Placeholder for app/game tabs
        if (IsKeyPressed(KEY_TAB)) {
            settingsTab = (settingsTab + 1) % 2;        // expand later to 3
        }
        if (IsKeyPressed(KEY_F)) {
            ToggleFullscreen();
        }
    }
}

static void drawSettings(void) {
    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(BLACK, 0.8f));

    DrawTextEx(fonts[FONT60], "SETTINGS", (Vector2){GetScreenWidth()/2.0f - MeasureTextEx(fonts[FONT60], "SETTINGS", 60, 2).x/2, 100}, 60, 2, WHITE);

    Color colors[2] = { (settingsTab == 0) ? YELLOW : GRAY, (settingsTab == 1) ? YELLOW : GRAY };
    DrawTextEx(fonts[FONT40], "App",  (Vector2){200, 180}, 40, 2, colors[0]);
    DrawTextEx(fonts[FONT40], "Game", (Vector2){400, 180}, 40, 2, colors[1]);
    DrawTextEx(fonts[FONT40], "Keybinds", (Vector2){400, 180}, 40, 2, colors[1]);
    
    if (currentState == STATE_SETTINGS_KEYBINDS) {
        drawKeybinds();
    }

    DrawTextEx(
        fonts[FONT20], "ESC: back   TAB: switch tab (when available)",
        (Vector2){GetScreenWidth()/2.0f - MeasureTextEx(fonts[FONT20], "...", 20, 1).x/2,
                         GetScreenHeight() - 80}, 20, 1, LIGHTGRAY);
}

//------------------------------------------------------------------------------------
static void updateKeybinds(void) {
    Vector2 mouse = GetMousePosition();

    for (s32 i = 0; i < __keybindActionCount; i++) {
        // Click to start rebinding
        Rectangle rowRect = {200, 240 + i*60, 600, 50};
        if (CheckCollisionPointRec(mouse, rowRect) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            rebindingAction = i;
        }
    }

    if (rebindingAction >= 0) {
        s32 key = GetKeyPressed();
        if (key > 0 && key != KEY_ESCAPE && key != KEY_LEFT_CONTROL) {      // avoid conflict keys
            currentKeybinds[rebindingAction] = key;
            rebindingAction = -1;
        }
        if (IsKeyPressed(KEY_ESCAPE)) {
            rebindingAction = -1;       // cancel
        }
    }
}

static void drawKeybinds(void) {
    const s32 baseY = 240;

    DrawTextEx(fonts[FONT40], "App Keybinds", (Vector2){200, 180}, 40, 2, WHITE);
    DrawTextEx(fonts[FONT40], "Game Keybinds", (Vector2){200, 360}, 40, 2, WHITE);

    // For simplicity draw all in one section; split later if needed
    for (s32 i = 0; i < __keybindActionCount; i++) {
        s32 y = baseY + i * 60;

        DrawTextEx(fonts[FONT30], keybindNames[i], (Vector2){220, y}, 30, 1, LIGHTGRAY);

        const char *keyText = (rebindingAction == i) ? "Press key..." : TextFormat("%c", (char)currentKeybinds[i]);
        Color col = (rebindingAction == i) ? ORANGE : WHITE;
        DrawTextEx(fonts[FONT30], keyText, (Vector2){700, y}, 30, 1, col);
    }

    if (rebindingAction >= 0) {
        DrawTextEx(fonts[FONT24], "ESC to cancel", (Vector2) {GetScreenWidth()/2.0f - 100, GetScreenHeight() - 150}, 24, 1, YELLOW);
    }
}