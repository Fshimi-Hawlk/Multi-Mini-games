/**
    @file main.c
    @author Fshimi-Hawlk
    @date 2026-03-02
    @date 2026-03-03
    @brief One clear sentence that tells what this file is actually for.
    
    Contributors:
        - Fshimi-Hawlk:
            - Provided documentation start-up
            - Initial implementation
            - Refactored into structured state and helper functions

    @note Uses Raylib for rendering and input.
          Numbers 0-99 placed randomly without column restrictions.
          Calls use B/I/N/G/O format but do not enforce column ranges.
*/

#include "core/game.h"

#include "setups/app.h"
#include "setups/game.h"

#include "ui/app.h"
#include "ui/game.h"

#include "utils/globals.h"

// ────────────────────────────────────────────────
// Entry point
// ────────────────────────────────────────────────

int main(void) {
    if (!initApp()) {
        log_fatal("Application initialization failed. Aborting.");
        freeApp();
        return 1;
    }

    BingoGame_St game = {0};

    bingo_initGame(&game);

    while (!WindowShouldClose()) {
        f32Vector2 mouse = GetMousePosition();
        f32 dt = GetFrameTime();

        bingo_updateGame(&game, dt, mouse);

        BeginDrawing(); {
            ClearBackground(APP_BACKGROUND_COLOR);

            switch (game.progress.scene) {
                case GAME_SCENE_CARD_CHOICE: {
                    bingo_drawChoiceCards(&game);
                } break;

                case GAME_SCENE_LAUNCHING: {
                    bingo_drawChoiceCards(&game);

                    char text[2] = {0};
                    sprintf(text, "%.0f", game.currentCall.timer / 2);
                    f32Vector2 textSize = MeasureTextEx(fonts[FONT48], text, 128, 0);

                    DrawTextEx(
                        fonts[FONT48], text,
                        Vector2Subtract(game.layout.windowCenter, Vector2Scale(textSize, 0.5)), 
                        128, 0, BLACK
                    );
                } break;

                case GAME_SCENE_PLAYING: {
                    bingo_drawCard(&game);
                    bingo_drawUI(&game);
                } break;

                case GAME_SCENE_END: {
                    f32 fontSize = 64;
                    u32 w = MeasureText(game.progress.resultMessage, fontSize);
                    Color col = (game.progress.resultMessage[0] == 'B') ? GREEN : RED;

                    f32Vector2 textPos = {
                        .x = game.layout.windowCenter.x - w / 2.0f,
                        .y = game.layout.windowCenter.y - fontSize / 2.0f,
                    };

                    DrawTextEx(fonts[FONT48], game.progress.resultMessage, textPos, fontSize, 0, col);
                } break;
            }

    
            // Debug timer (remove later)
            // DrawText(TextFormat("%.2f", game.currentCall.timer), 10, 10, 16, BLACK);
        } EndDrawing();
    }

    freeApp();
    return 0;
}

#define LOGGER_IMPLEMENTATION
#include "logger.h"

// Required when contextArena.h is used for custom allocators
// #define CONTEXT_ARENA_IMPLEMENTATION
// #include "contextArena.h"