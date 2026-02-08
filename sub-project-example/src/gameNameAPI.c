#include "utils/common.h"
#include "utils/userTypes.h"

#define GAMENAME_API_IMPLEMENTATION
#include "gameNameAPI.h"

// ─────────────────────────────────────────────────────────────────────────────
// Private internal structure definition
// Only visible inside this translation unit
// ─────────────────────────────────────────────────────────────────────────────

struct GameNameGame_St {
    Game_St base;              // Must be first — allows safe cast to Game_St*

    // Game-specific fields
    // int someCounter;
    // Texture2D playerSprite;
    // ... add real fields here
};

// ─────────────────────────────────────────────────────────────────────────────
// Implementation
// ─────────────────────────────────────────────────────────────────────────────

Error_Et gameName_initGame__full(GameNameGame_St** game, GameNameConfigs_St configs) {
    if (game == NULL) {
        log_error("NULL game double-pointer passed to init");
        return ERROR_NULL_POINTER;
    }

    *game = calloc(1, sizeof(**game));
    if (*game == NULL) {
        log_error("Failed to allocate GameNameGame_St");
        return ERROR_ALLOC;
    }

    // Initialize common base fields
    (*game)->base.running = true;

    // Apply custom configuration
    if (configs.fps > 0) {
        SetTargetFPS(configs.fps);
    }

    // Initialize game-specific resources
    // (*game)->playerSprite = LoadTexture("assets/player.png");
    // if ((*game)->playerSprite.id == 0) { ... error handling ... }

    log_info("GameName initialized successfully (FPS: %u)", configs.fps ? configs.fps : 60);
    return OK;
}

void gameName_gameLoop(GameNameGame_St* const game) {
    if (game == NULL) {
        log_error("NULL game pointer in gameLoop");
        return;
    }

    if (!game->base.running) {
        return;
    }

    // ── Input ────────────────────────────────────────────────────────────────
    // ...

    // ── Update ───────────────────────────────────────────────────────────────
    // ...

    // ── Draw ─────────────────────────────────────────────────────────────────
    BeginDrawing();
    {
        ClearBackground(RAYWHITE);

        // Draw game elements...
        // DrawTexture(game->playerSprite, x, y, WHITE);
    }
    EndDrawing();

    // Example end condition
    if (IsKeyPressed(KEY_ESCAPE)) {
        game->base.running = false;
    }
}

void gameName_freeGame(GameNameGame_St** game)
{
    if (game == NULL || *game == NULL) {
        return;
    }

    // Unload game-specific resources
    // UnloadTexture((*game)->playerSprite);

    free(*game);
    *game = NULL;

    log_debug("GameName resources freed");
}