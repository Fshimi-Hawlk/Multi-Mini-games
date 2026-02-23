/**
    @file gameNameAPI.c
    @author Fshimi-Hawlk
    @date 2026-01-25              // Creation date - never touch this one again
    @date 2026-02-23              // Last time someone meaningfully changed this file (update only when needed)
    @brief One clear sentence that tells what this file is actually for.
  
    Contributors:
        - Fshimi-Hawlk:
            - Provided documentation start-up
        - <Name>:
            - What you added / changed / fixed (keep it short)
  
    If the file needs more context than fits in @brief, write 2-5 lines here.
    @note Put warnings, important limitations, "we know it's ugly but...", or future plans here

    // Try to align the `for` for better readability
    // Try to keep the same order of the includes
    Use @see `path/to/related/file.h` when this file depends heavily on another one.
*/

#include "utils/userTypes.h"

#include "APIs/generalAPI.h"
#include "gameNameAPI.h"

// ─────────────────────────────────────────────────────────────────────────────
// Private internal structure definition
// Only visible inside this translation unit
// ─────────────────────────────────────────────────────────────────────────────

struct GameNameGame_St {
    Game_St base;              // Must be first - allows safe cast to Game_St*

    // Game-specific fields
    // int someCounter;
    // Texture2D playerSprite;
    // ... add real fields here
};

// ─────────────────────────────────────────────────────────────────────────────
// Implementation
// ─────────────────────────────────────────────────────────────────────────────

/// @note: that wrapper serve as an interface to avoid any annoying warning
Error_Et gameName_freeGameWrapper(void* game) {
    return gameName_freeGame((GameNameGame_St**) game);
}

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

    // Have a game reference to avoid constant deferencement
    GameNameGame_St* gameRef = *game;

    // Initialize common base fields
    gameRef->base.running = true;

    /// @note: if `configs` isn't used in this function, 
    ///        to avoid warning, put at the top of the 
    ///        function `(void) configs`
    // Apply custom configuration
    if (configs.fps > 0) {
        SetTargetFPS(configs.fps);
    }

    // Initialize game-specific resources
    // gameRef->playerSprite = LoadTexture("assets/player.png");
    // if (gameRef->playerSprite.id == 0) { ... error handling ... }

    log_info("GameName initialized successfully (FPS: %u)", configs.fps ? configs.fps : 60);
    return OK;
}

Error_Et gameName_gameLoop(GameNameGame_St* const game) {
    if (game == NULL) {
        log_debug("NULL game pointer in gameLoop");
        return ERROR_NULL_POINTER;
    }

    if (!game->base.running) {
        return OK;
    }

    // ── Input ────────────────────────────────────────────────────────────────
    // ...

    // ── Update ───────────────────────────────────────────────────────────────
    // ...

    // ── Draw ─────────────────────────────────────────────────────────────────
    BeginDrawing(); {
        ClearBackground(RAYWHITE);

        // Draw game elements...

    } EndDrawing();

    return OK;
}

Error_Et gameName_freeGame(GameNameGame_St** game) {
    if (game == NULL || *game == NULL) {
        return ERROR_NULL_POINTER;
    }

    GameNameGame_St* gameRef = *game;

    // Unload game-specific resources
    // e.g. UnloadTexture(gameRef->playerSprite);

    free(gameRef);
    *game = NULL;

    log_debug("GameName resources freed");
    return OK;
}