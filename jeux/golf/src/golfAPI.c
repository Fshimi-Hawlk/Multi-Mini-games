/**
    @file golfAPI.c
    @brief Lobby-compatible lifecycle wrapper for Golf 3D.

    Exposes golf_initGame / golf_gameLoop / golf_freeGame following the
    same contract as every other mini-game (bowlingAPI.c, tetrisAPI.c …).

    The concrete type `GolfGame_St` has `BaseGame_St base` as its first
    member, which allows the lobby to safely cast `BaseGame_St* ↔ GolfGame_St*`.
*/

#include "golfAPI.h"
#include "golf.h"

#include <stdlib.h>
#include <string.h>

/* ─── Concrete (private) type ────────────────────────────────────────────── */

struct GolfGame_St {
    BaseGame_St base;   /* MUST be first — lobby casts through this */
    GolfGame    game;   /* full internal golf state                  */
};

/* ─── Cleanup callback (stored in base.freeGame) ────────────────────────── */

static Error_Et golf_free_callback(void *g) {
    GolfGame_St *self = (GolfGame_St *)g;
    if (!self) return ERROR_NULL_POINTER;
    Game_Cleanup(&self->game);
    free(self);
    return OK;
}

/* ─── API implementation ─────────────────────────────────────────────────── */

Error_Et golf_initGame__full(GolfGame_St **out, GolfConfigs_St configs) {
    GolfGame_St *g;

    if (!out) return ERROR_NULL_POINTER;

    g = (GolfGame_St *)malloc(sizeof(GolfGame_St));
    if (!g) return ERROR_ALLOC;
    memset(g, 0, sizeof(*g));

    Game_Init(&g->game);

    g->base.running  = true;
    g->base.paused   = false;
    g->base.score    = 0;
    g->base.freeGame = golf_free_callback;

    (void)configs; /* reserved for future use */

    *out = g;
    return OK;
}

Error_Et golf_gameLoop(GolfGame_St *const g) {
    float dt;

    if (!g) return ERROR_NULL_POINTER;

    dt = GetFrameTime();
    if (dt > 0.05f) dt = 0.05f;

    /* Scorecard visible: ESC ou SPACE → fin de partie, retour lobby */
    if (g->game.state == STATE_SCORECARD) {
        if (IsKeyPressed(KEY_ESCAPE) || IsKeyPressed(KEY_SPACE) ||
            IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            g->base.running = false;
            return OK;
        }
    }

    Game_Update(&g->game, dt);
    Game_Draw(&g->game);

    return OK;
}

Error_Et golf_freeGame(GolfGame_St **g) {
    if (!g || !*g) return ERROR_NULL_POINTER;
    Game_Cleanup(&(*g)->game);
    free(*g);
    *g = NULL;
    return OK;
}
