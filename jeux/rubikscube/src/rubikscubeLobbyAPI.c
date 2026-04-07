/**
 * @file rubikscubeLobbyAPI.c
 * @brief Wrapper lobby pour Rubik's Cube (BaseGame_St pattern).
 */
#include "rubiksCubeAPI.h"
#include "logger.h"
#include "raylib.h"
#include <stdlib.h>

struct RubiksCubeAPI_St {
    BaseGame_St         base;
    RubiksCubeGame_St*  inner;
};

static Error_Et rc_freeWrapper(void* g) {
    RubiksCubeAPI_St* h = (RubiksCubeAPI_St*)g;
    return rubikscube_freeGame(&h);
}

Error_Et rubikscube_initGame__full(RubiksCubeAPI_St** game, RubiksCubeConfigs_St cfg) {
    if (!game) return ERROR_NULL_POINTER;
    *game = calloc(1, sizeof(RubiksCubeAPI_St));
    if (!*game) return ERROR_ALLOC;
    RubiksCubeAPI_St* g = *game;
    g->base.freeGame = rc_freeWrapper;
    g->base.running  = true;
    VideoConfig_St video = {
        .width      = cfg.width > 0 ? cfg.width : 1200,
        .height     = cfg.height > 0 ? cfg.height : 800,
        .fps        = cfg.fps > 0 ? (int)cfg.fps : GAME_DEFAULT_FPS,
        .fullscreen = GAME_DEFAULT_FULLSCREEN,
        .vsync      = GAME_DEFAULT_VSYNC,
        .title      = "Rubik's Cube",
    };
    GameConfig_St inner = { .audio = NULL, .video = &video };
    g->inner = rubiksCube_initGame(&inner);
    if (!g->inner) { free(g); *game = NULL; return ERROR_ALLOC; }
    log_debug("RubiksCube: initialized");
    return OK;
}

Error_Et rubikscube_gameLoop(RubiksCubeAPI_St* const g) {
    if (!g)               return ERROR_NULL_POINTER;
    if (!g->base.running) return OK;
    if (IsKeyPressed(KEY_ESCAPE)) { g->base.running = false; return OK; }
    rubiksCube_gameLoop(g->inner);
    if (!rubiksCube_isRunning(g->inner))
        g->base.running = false;
    return OK;
}

Error_Et rubikscube_freeGame(RubiksCubeAPI_St** game) {
    if (!game || !*game) return ERROR_NULL_POINTER;
    if ((*game)->inner) rubiksCube_freeGame((*game)->inner);
    free(*game); *game = NULL;
    return OK;
}

bool rubikscube_isRunning(const RubiksCubeAPI_St* g) { return g && g->base.running; }
