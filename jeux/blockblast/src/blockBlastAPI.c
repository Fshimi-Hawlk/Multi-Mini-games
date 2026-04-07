/**
    @file blockBlastAPI.c
    @author Multi Mini-Games Team
    @date 2026-04-05
    @brief Lobby wrapper for the Block-Blast mini-game.

    BlockBlastAPI_St wraps BaseGame_St + internal GameState_St.
    The window is owned by the lobby. We manage: arenas, fonts, audio,
    prefab bags, game state.

    Implementation-note: block-blast uses contextArena / rand — we define
    their IMPLEMENTATION macros here (single TU that owns the singletons).
*/

/* ── Implementation macros (must be first, single TU) ─────────────────── */
#define CONTEXT_ARENA_IMPLEMENTATION
#include "contextArena.h"

#define RAND_IMPLEMENTATION
#include "rand.h"

/* ── Block-blast headers ───────────────────────────────────────────────── */
#include "utils/common.h"
#include "utils/globals.h"
#include "utils/configs.h"
#include "utils/audio.h"

#include "core/app.h"
#include "core/game.h"
#include "core/save.h"
#include "core/shape.h"
#include "core/placement.h"

#include "ui/game.h"

/* ── Shared API headers ────────────────────────────────────────────────── */
#include "APIs/generalAPI.h"
#include "APIs/blockBlastAPI.h"

#include "logger.h"
#include "raylib.h"

#include <stdlib.h>
#include <string.h>
#include <time.h>

/* ────────────────────────────────────────────────────────────────────────────
   Concrete opaque handle
   ──────────────────────────────────────────────────────────────────────────── */
struct BlockBlastAPI_St {
    BaseGame_St  base;        /* MUST be first */
    GameState_St state;       /* full block-blast game state */
    f64          prevScore;   /* score at start of last turn (for adjustSizeWeights) */
};

/* ── freeGame callback ─────────────────────────────────────────────────── */
static Error_Et blockBlast_freeGameWrapper(void* g) {
    /* g is BaseGame_St** == BlockBlastAPI_St** (base is first member).
     * Cast directly to BlockBlastAPI_St** so blockBlast_freeGame can set *g = NULL. */
    return blockBlast_freeGame((BlockBlastAPI_St**)g);
}

/* ── Asset path for fonts ─────────────────────────────────────────────── */
#ifndef BB_FONT_PATH
#define BB_FONT_PATH "assets/fonts/Nunito/Nunito-Black.ttf"
#endif

static void bb_loadFonts(void) {
    u64 fontSize = 8;
    for (u64 id = 0; id < _fontSizeCount; id++, fontSize += 2) {
        bb_fonts[id] = LoadFontEx(BB_FONT_PATH, (float)fontSize, NULL, 0);
        if (!IsFontValid(bb_fonts[id]))
            log_warn("BlockBlast: font id=%zu not loaded", id);
    }
}

static void bb_unloadFonts(void) {
    for (u64 id = 0; id < _fontSizeCount; id++)
        UnloadFont(bb_fonts[id]);
}

/* ── Init ─────────────────────────────────────────────────────────────── */
Error_Et blockBlast_initGame__full(BlockBlastAPI_St** game, BlockBlastConfigs_St configs) {
    (void)configs;
    if (!game) return ERROR_NULL_POINTER;

    *game = calloc(1, sizeof(BlockBlastAPI_St));
    if (!*game) { log_error("BlockBlast: alloc failed"); return ERROR_ALLOC; }

    BlockBlastAPI_St* g = *game;
    g->base.freeGame = blockBlast_freeGameWrapper;
    g->base.running  = true;
    g->prevScore     = 0.0;

    /* Seed RNG */
    u64 seeds[2] = {(u64)time(NULL), (u64)(uintptr_t)g};
    prng_seed(seeds[0], seeds[1]);

    blockBlast_initAudio();
    bb_loadFonts();
    initGame(&g->state, false);

    log_debug("BlockBlast: initialized");
    return OK;
}

/* ── Game loop ────────────────────────────────────────────────────────── */
Error_Et blockBlast_gameLoop(BlockBlastAPI_St* const g) {
    if (!g)               return ERROR_NULL_POINTER;
    if (!g->base.running) return OK;

    if (IsKeyPressed(KEY_ESCAPE)) { g->base.running = false; return OK; }

    /* Update score in base (visible to lobby) */
    g->base.score = (long)g->state.scoring.score;

    if (!g->state.gameOver) {
        bool allPlaced = true;
        for (u8 i = 0; i < 3; ++i) {
            handleShape(&g->state, &g->state.prefabManager.slots[i]);
            allPlaced &= g->state.prefabManager.slots[i].placed;
        }

        if (allPlaced) {
            adjustSizeWeights(&g->state, g->state.scoring.score - g->prevScore);
            shuffleSlots(&g->state.prefabManager);
            placementSimulation(&g->state);
            g->prevScore = g->state.scoring.score;
        }
    } else {
        /* Game over: press R to restart or ESC to quit */
        if (IsKeyPressed(KEY_R)) {
            initGame(&g->state, false);
            g->prevScore = 0.0;
        }
    }

    BeginDrawing(); {
        ClearBackground(APP_BACKGROUND_COLOR);
        drawUI(&g->state);
        if (g->state.gameOver) {
            int sw = GetScreenWidth();
            int sh = GetScreenHeight();
            DrawRectangle(sw/2 - 180, sh/2 - 60, 360, 120,
                          (Color){0,0,0,200});
            DrawText("GAME OVER",
                     sw/2 - MeasureText("GAME OVER", 40)/2,
                     sh/2 - 40, 40, RED);
            DrawText("R: Rejouer  ESC: Quitter",
                     sw/2 - MeasureText("R: Rejouer  ESC: Quitter", 18)/2,
                     sh/2 + 10, 18, LIGHTGRAY);
        }
    } EndDrawing();

    return OK;
}

/* ── Free ─────────────────────────────────────────────────────────────── */
Error_Et blockBlast_freeGame(BlockBlastAPI_St** game) {
    if (!game || !*game) return ERROR_NULL_POINTER;

    bb_unloadFonts();
    blockBlast_freeAudio();

    /* All dynamic arrays (bags[].items, prefabsBag.items) are allocated via
     * context_realloc → arena.  arena_free() releases all of them.
     * Calling free() on those pointers afterwards would be a double-free. */
    arena_free(&globalArena);
    arena_free(&tempArena);

    free(*game);
    *game = NULL;
    return OK;
}

bool blockBlast_isRunning(const BlockBlastAPI_St* game) {
    return game && game->base.running;
}
