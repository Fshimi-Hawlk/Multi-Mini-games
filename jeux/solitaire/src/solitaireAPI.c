/**
 * @file solitaireAPI.c
 * @brief Solitaire public API — mirrors tetrisAPI pattern exactly.
 *
 * The lobby owns the window. This file: malloc, init, draw, free.
 * No InitWindow, no CloseWindow, no SetWindowSize.
 */

#include "solitaireAPI.h"
#include "solitaire.h"
<<<<<<< HEAD
#include "audio.h"

#include "APIs/generalAPI.h"
#include <stdlib.h>
#include <string.h>

/* ── Concrete state ───────────────────────────────────────────── */

=======

// Include raylib first to avoid type conflicts
#include <raylib.h>

// Define struct before params menu
>>>>>>> 3777fd6 (- add : new 3D golf game)
struct SolitaireGame_St {
    BaseGame_St        base;   /* MUST be first — safe cast to BaseGame_St* */
    SolitaireGameState state;
};

<<<<<<< HEAD
=======
// Include params menu header for type definition (not implementation)
#include "../../../firstparty/include/ui/paramsMenu.h"

#include "audio.h"
#include "APIs/generalAPI.h"
#include <stdlib.h>
#include <string.h>

// Global params menu state for solitaire
static ParamsMenu_St solitaireParamsMenu = {0};

/* ── Concrete state ───────────────────────────────────────────── */

>>>>>>> 3777fd6 (- add : new 3D golf game)
/* ── freeGame callback (called by lobby via base.freeGame) ──── */

static Error_Et solitaire_freeGameWrapper(void* game) {
    return solitaire_freeGame((SolitaireGame_St**)game);
}

/* ── Lifecycle ──────────────────────────────────────────────── */

Error_Et solitaire_initGame__full(SolitaireGame_St** game_ptr,
                                  const GameConfig_St* config) {
    (void)config;
    if (!game_ptr) return ERROR_NULL_POINTER;

    *game_ptr = malloc(sizeof(SolitaireGame_St));
    if (!*game_ptr) return ERROR_ALLOC;

    SolitaireGame_St* g = *game_ptr;
    memset(g, 0, sizeof(*g));

    g->base.running  = true;
    g->base.freeGame = solitaire_freeGameWrapper;

    /* solitaire_init handles card setup + asset loading internally */
    solitaire_init(&g->state);
    solitaire_initAudio();

<<<<<<< HEAD
=======
    // Initialize params menu (settings button)
    paramsMenu_init(&solitaireParamsMenu);

>>>>>>> 3777fd6 (- add : new 3D golf game)
    return OK;
}

void solitaire_gameLoop(SolitaireGame_St* const game) {
    if (!game || !game->base.running) return;

    if (IsKeyPressed(KEY_ESCAPE)) {
        game->base.running = false;
        return;
    }

<<<<<<< HEAD
=======
    // Update params menu (settings button clicks)
    paramsMenu_update(&solitaireParamsMenu);

>>>>>>> 3777fd6 (- add : new 3D golf game)
    float dt = GetFrameTime();
    solitaire_update(&game->state, dt);

    BeginDrawing();
        ClearBackground((Color){34, 85, 51, 255});
        solitaire_draw(&game->state);
<<<<<<< HEAD
=======

        // Draw params menu (settings button)
        paramsMenu_draw(&solitaireParamsMenu);
>>>>>>> 3777fd6 (- add : new 3D golf game)
    EndDrawing();
}

Error_Et solitaire_freeGame(SolitaireGame_St** game_ptr) {
    if (!game_ptr || !*game_ptr) return ERROR_NULL_POINTER;

    solitaire_freeAudio();
    solitaire_cleanup(&(*game_ptr)->state);

<<<<<<< HEAD
=======
    // Cleanup params menu
    paramsMenu_free(&solitaireParamsMenu);

>>>>>>> 3777fd6 (- add : new 3D golf game)
    free(*game_ptr);
    *game_ptr = NULL;
    return OK;
}

bool solitaire_isRunning(const SolitaireGame_St* game) {
    return game && game->base.running;
}
