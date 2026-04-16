/**
    @file golfAPI.c
    @author Maxime CHAUVEAU
    @date 2026-04-01
    @date 2026-04-14
    @brief Lobby-compatible lifecycle wrapper for Golf 3D.
*/
#include "golfAPI.h"
#include "golf.h"

#include <stdlib.h>
#include <string.h>

/* ─── Concrete (private) type ────────────────────────────────────────────── */

/**
    @brief Concrete implementation of the opaque GolfGame_St handle.
*/
struct GolfGame_St {
    BaseGame_St base;   ///< MUST be first — lobby casts through this
    GolfGame    game;   ///< full internal golf state
};

/* ─── Cleanup callback (stored in base.freeGame) ────────────────────────── */

/**
    @brief Internal callback to free the game instance, compatible with BaseGame_St.
    
    @param[in,out] g Generic pointer to the GolfGame_St instance.
    @return OK on success, ERROR_NULL_POINTER if g is NULL.
*/
static Error_Et golf_free_callback(void *g) {
    GolfGame_St *self = (GolfGame_St *)g;
    if (!self) return ERROR_NULL_POINTER;
    Game_Cleanup(&self->game);
    free(self);
    return OK;
}

/* ─── API implementation ─────────────────────────────────────────────────── */

/**
    @brief Allocates and initializes a new Golf 3D game instance.

    @param[out] out      Double pointer receiving the new game handle.
    @param[in]  configs  Initialization options.
    @return              OK on success, ERROR_ALLOC on memory failure, ERROR_NULL_POINTER if out is NULL.
*/
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

/**
    @brief Runs one complete frame: input → logic → rendering.

    @param[in,out] g  Valid game instance handle.
    @return           OK on success, ERROR_NULL_POINTER if g is NULL.
*/
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

/**
    @brief Frees all resources and releases the handle.

    @param[in,out] g  Pointer to game handle (set to NULL after cleanup).
    @return           OK on success, ERROR_NULL_POINTER if *g is invalid.
*/
Error_Et golf_freeGame(GolfGame_St **g) {
    if (!g || !*g) return ERROR_NULL_POINTER;
    Game_Cleanup(&(*g)->game);
    free(*g);
    *g = NULL;
    return OK;
}
