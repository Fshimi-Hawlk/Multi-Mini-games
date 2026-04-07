/**
    @file bingoAPI.c
    @author Multi Mini-Games Team
    @date 2026-04-05
    @brief Lobby wrapper for the Bingo mini-game.

    BingoAPI_St wraps BaseGame_St + the original BingoGame_St state.
*/

#include "utils/userTypes.h"
#include "utils/globals.h"
#include "utils/configs.h"

#include "core/game.h"
#include "core/game_init.h"
#include "core/bingoAPI.h"
#include "ui/app.h"
#include "ui/game.h"

#include "APIs/generalAPI.h"

#include "logger.h"
#include "raylib.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/* ── Concrete opaque handle ──────────────────────────────────────────── */
struct BingoAPI_St {
    BaseGame_St  base;   /* MUST be first */
    BingoGame_St game;
};

static Error_Et bingo_freeGameWrapper(void* g) {
    /* g is BaseGame_St** == BingoAPI_St** (base is first member).
     * Cast directly to BingoAPI_St** so bingo_freeGame can set *g = NULL. */
    return bingo_freeGame((BingoAPI_St**)g);
}

/* ── Font helpers ─────────────────────────────────────────────────────── */
#ifndef BINGO_ASSET_PATH
#define BINGO_ASSET_PATH "jeux/bingo/assets/"
#endif

static void bingo_loadFonts(void) {
    u64 sz = 4;
    for (u64 id = 0; id < __fontSizeCount; id++, sz += 2) {
        bingo_fonts[id] = LoadFontEx(
            BINGO_ASSET_PATH "fonts/Noto/static/NotoSansMono-Bold.ttf",
            (float)sz, NULL, 0);
        if (!IsFontValid(bingo_fonts[id]))
            log_warn("Bingo: font id=%zu not loaded", id);
    }
}

static void bingo_unloadFonts(void) {
    for (u64 id = 0; id < __fontSizeCount; id++)
        UnloadFont(bingo_fonts[id]);
}

/* Prevent the macro from intercepting our call to the internal setup function */
#undef bingo_initGame

/* ── Init ─────────────────────────────────────────────────────────────── */
Error_Et bingo_initGame__full(BingoAPI_St** game, BingoConfigs_St configs) {
    (void)configs;
    if (!game) return ERROR_NULL_POINTER;

    *game = calloc(1, sizeof(BingoAPI_St));
    if (!*game) { log_error("Bingo: alloc failed"); return ERROR_ALLOC; }

    BingoAPI_St* g = *game;
    g->base.freeGame = bingo_freeGameWrapper;
    g->base.running  = true;

    bingo_loadFonts();
    bingo_initGame(&g->game);

    log_debug("Bingo: initialized");
    return OK;
}

/* ── Game loop ────────────────────────────────────────────────────────── */
Error_Et bingo_gameLoop(BingoAPI_St* const g) {
    if (!g)               return ERROR_NULL_POINTER;
    if (!g->base.running) return OK;

    if (IsKeyPressed(KEY_ESCAPE)) { g->base.running = false; return OK; }

    f32Vector2 mouse = GetMousePosition();
    f32        dt    = GetFrameTime();

    bingo_updateGame(&g->game, dt, mouse);

    if (g->game.progress.scene == GAME_SCENE_END) {
        static float endTimer = 0.0f;
        endTimer += dt;
        if (endTimer >= 3.0f || IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) {
            endTimer = 0.0f;
            g->base.running = false;
        }
    }

    BeginDrawing(); {
        ClearBackground(APP_BACKGROUND_COLOR);

        switch (g->game.progress.scene) {
            case GAME_SCENE_CARD_CHOICE:
                bingo_drawChoiceCards(&g->game);
                break;

            case GAME_SCENE_LAUNCHING: {
                bingo_drawChoiceCards(&g->game);
                char buf[8];
                snprintf(buf, sizeof(buf), "%.0f", g->game.currentCall.timer / 2.0f);
                f32Vector2 sz2 = MeasureTextEx(bingo_fonts[FONT48], buf, 128, 0);
                DrawTextEx(bingo_fonts[FONT48], buf,
                    Vector2Subtract(g->game.layout.windowCenter, Vector2Scale(sz2, 0.5f)),
                    128, 0, BLACK);
            } break;

            case GAME_SCENE_PLAYING:
                bingo_drawCard(&g->game);
                bingo_drawUI(&g->game);
                break;

            case GAME_SCENE_END: {
                const char* msg = g->game.progress.resultMessage
                                ? g->game.progress.resultMessage : "";
                int w = MeasureText(msg, 64);
                Color col = (msg[0] == 'B') ? GREEN : RED;
                DrawText(msg,
                    (int)(g->game.layout.windowCenter.x - w / 2.0f),
                    (int)(g->game.layout.windowCenter.y - 32.0f),
                    64, col);
                DrawText("ENTREE / ESC pour continuer",
                    (int)(g->game.layout.windowCenter.x - 150),
                    (int)(g->game.layout.windowCenter.y + 55),
                    20, DARKGRAY);
            } break;
        }
    } EndDrawing();

    return OK;
}

/* ── Free ─────────────────────────────────────────────────────────────── */
Error_Et bingo_freeGame(BingoAPI_St** game) {
    if (!game || !*game) return ERROR_NULL_POINTER;
    bingo_unloadFonts();
    free(*game);
    *game = NULL;
    return OK;
}

bool bingo_isRunning(const BingoAPI_St* game) {
    return game && game->base.running;
}
