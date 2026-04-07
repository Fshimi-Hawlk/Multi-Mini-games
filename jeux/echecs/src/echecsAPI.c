/**
 * @file echecsAPI.c
 * @brief Wrapper lobby pour Echecs (Chess) — BaseGame_St pattern.
 */
#include "echecsAPI.h"
#include "core/game.h"
#include "utils/global.h"
#include "APIs/generalAPI.h"
#include "logger.h"
#include <stdlib.h>
#include <string.h>

/* Global board — echecs uses a global Board_t */
static Board_t g_board;

struct EchecsAPI_St {
    BaseGame_St base;
};

static Error_Et echecs_freeWrapper(void* g) {
    EchecsAPI_St* h = (EchecsAPI_St*)g;
    return echecs_freeGame(&h);
}

Error_Et echecs_initGame__full(EchecsAPI_St** game, EchecsConfigs_St cfg) {
    (void)cfg;
    if (!game) return ERROR_NULL_POINTER;
    *game = calloc(1, sizeof(EchecsAPI_St));
    if (!*game) return ERROR_ALLOC;
    EchecsAPI_St* g = *game;
    g->base.freeGame = echecs_freeWrapper;
    g->base.running  = true;
    memset(g_board, 0, sizeof(g_board));
    if (initGame(g_board) != 0) {
        free(g); *game = NULL;
        return ERROR_ALLOC;
    }
    resetGame();
    running  = true;
    finished = false;
    log_debug("Echecs: initialized");
    return OK;
}

Error_Et echecs_gameLoop(EchecsAPI_St* const g) {
    if (!g)               return ERROR_NULL_POINTER;
    if (!g->base.running) return OK;
    if (IsKeyPressed(KEY_ESCAPE)) {
        g->base.running = false;
        running         = false;
        return OK;
    }
    echecsGameOneFrame(g_board);
    if (!running || WindowShouldClose()) {
        g->base.running = false;
    }
    return OK;
}

Error_Et echecs_freeGame(EchecsAPI_St** game) {
    if (!game || !*game) return ERROR_NULL_POINTER;
    freeGame();
    free(*game); *game = NULL;
    return OK;
}

bool echecs_isRunning(const EchecsAPI_St* g) { return g && g->base.running; }
