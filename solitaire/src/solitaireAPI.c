/**
 * @file solitaireAPI.c
 * @brief Solitaire API implementation
 * @author Maxime CHAUVEAU
 * @date February 2026
 */

#include "solitaireAPI.h"
#include "solitaire.h"
#include "ui/renderer.h"
#include <stdlib.h>
#include <string.h>
#include <raylib.h>

#define SOLITAIRE_DEFAULT_WIDTH 1200
#define SOLITAIRE_DEFAULT_HEIGHT 800
#define SOLITAIRE_DEFAULT_FPS 60

struct SolitaireGame_St {
    SolitaireGameState gameState;
    GameAssets assets;
    bool running;
    bool windowInitialized;
    int originalWidth;
    int originalHeight;
};

static void applyVideoConfig(const VideoConfig_St* video) {
    if (!video) return;
    
    if (video->fps > 0) {
        SetTargetFPS(video->fps);
    }
    if (video->fullscreen) {
        ToggleFullscreen();
    }
    if (video->vsync) {
        SetWindowState(FLAG_VSYNC_HINT);
    }
}

SolitaireGame_St* solitaire_initGame(const GameConfig_St* config) {
    SolitaireGame_St* game = calloc(1, sizeof(*game));
    if (!game) return NULL;
    
    game->originalWidth = GetScreenWidth();
    game->originalHeight = GetScreenHeight();
    
    int width = SOLITAIRE_DEFAULT_WIDTH;
    int height = SOLITAIRE_DEFAULT_HEIGHT;
    const char* title = "Solitaire";
    
    if (config && config->video) {
        if (config->video->width > 0) width = config->video->width;
        if (config->video->height > 0) height = config->video->height;
        if (config->video->title) title = config->video->title;
    }
    
    if (!IsWindowReady()) {
        InitWindow(width, height, title);
        game->windowInitialized = true;
    } else {
        SetWindowSize(width, height);
        SetWindowTitle(title);
    }
    
    SetTargetFPS(SOLITAIRE_DEFAULT_FPS);
    
    if (config) {
        applyVideoConfig(config->video);
    }
    
    game->assets = LoadAssets();
    
    if (game->assets.cardSheet.id == 0 || game->assets.cardBack.id == 0) {
        UnloadAssets(game->assets);
        if (game->windowInitialized) CloseWindow();
        free(game);
        return NULL;
    }
    
    solitaire_init(&game->gameState);
    game->gameState.assets = malloc(sizeof(GameAssets));
    if (game->gameState.assets) {
        *game->gameState.assets = game->assets;
    }
    
    game->running = true;
    
    return game;
}

void solitaire_gameLoop(SolitaireGame_St* const game) {
    if (!game || !game->running) return;
    
    if (WindowShouldClose()) {
        game->running = false;
        return;
    }
    
    float deltaTime = GetFrameTime();
    
    solitaire_update(&game->gameState, deltaTime);
    
    BeginDrawing();
        ClearBackground((Color){34, 85, 51, 255});
        RenderGame(&game->gameState, game->assets);
        
        if (game->gameState.isWon) {
            RenderMenu(&game->gameState, game->assets);
        }
    EndDrawing();
}

void solitaire_freeGame(SolitaireGame_St* game) {
    if (!game) return;
    
    if (game->gameState.assets) {
        free(game->gameState.assets);
        game->gameState.assets = NULL;
    }
    
    UnloadAssets(game->assets);
    
    if (game->windowInitialized) {
        CloseWindow();
    } else if (IsWindowReady()) {
        SetWindowSize(game->originalWidth, game->originalHeight);
        SetWindowTitle("Multi Mini-Games");
    }
    
    free(game);
}

bool solitaire_isRunning(const SolitaireGame_St* game) {
    return game && game->running;
}
