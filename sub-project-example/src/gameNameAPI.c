/**
 * @file gamename.c
 * @author <Author's name>
 * @date <File creation date>
 * @date <File last update date>
 * @brief Implementation of the GameName mini-game.
 *
 * Contributors:
 *   - <Author's name>: // at the very least note what you did
 *      - What they provided
 *      - What they changed
 *   - <Contributor's name>: // if you contributed, note what you did
 *      - What they provided
 *      - What they changed
 *
 * Longer description if needed (2–5 lines max):
 *   - What this file/module does
 *   - Key types/functions it exposes
 *   - Important constraints or usage rules
 *   - Cross-references to related files (@see ...)
 *
 * // Try to align the `for` for better readability
 * // Try to keep the same order of the includes
 * @see `utils/userTypes.h`  for ...
 * @see `APIs/generalAPI.h`  for the general interface
 * @see `gameNameAPI.h`      for the public interface
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
        // DrawTexture(game->playerSprite, x, y, WHITE);
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