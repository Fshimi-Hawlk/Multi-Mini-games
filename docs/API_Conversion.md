# Converting a Sub-Game to an API and Integrating into the Lobby

**Last updated: February 08, 2026**  
**Author: Fshimi Hawlk**  
**Based on project structure as of Multi Mini-Games monorepo v[Unreleased]**

This guide provides a step-by-step explanation of how to convert an individual sub-game (e.g., `tetris/`, `block-blast/`) into a reusable API that can be integrated into the main lobby. The goal is to allow the lobby to launch and run the game seamlessly within the same window, without restarting the application or managing separate executables.

The process assumes you have a working standalone sub-game built with the project's Makefile system. It draws from the provided API examples (`generalAPI.h`, `gameNameAPI.h`, `gameNameAPI.c`) and lobby code (`main.c`). Key principles:
- Use **opaque structures** to hide internal game details from the lobby (encapsulation).
- Provide a standard interface: init, loop, free functions.
- Handle errors consistently with `Error_Et`.
- Ensure the game can run as a "scene" within the lobby's main loop.

## Prerequisites

- Your sub-game folder follows the template (`sub-project-example/`): `src/`, `include/`, `tests/`, `Makefile`.
- The game runs standalone: `make rebuild run-main` works without errors.
- Raylib is linked via the Makefile (e.g., `-lraylib` in `BASE_LDFLAGS`).
- Familiarity with the root Makefile: it builds libraries lazily and copies `<gamename>API.h` to `firstparty/APIs/`.
- Lobby code (`lobby/main.c`) uses a scene switcher (e.g., `currentScene`) to toggle between lobby and games.

If your game doesn't meet these, start by aligning it with the template.

## Step 1: Define the Game API Header (`include/<gamename>API.h`)

Create or update a public API header in your game's `include/` folder. This header exposes only what the lobby needs: opaque types, enums, and function prototypes. Do not expose internal structs or details—use forward declarations for opacity.

### Key Elements

1. **Include General API**:
   - Use `#include "../firstparty/APIs/generalAPI.h"` for shared types like `Error_Et` and `Game_St`.

2. **Opaque Game Structure**:
   - Declare `typedef struct <GameName>Game_St <GameName>Game_St;` (forward declaration).
   - This hides internals. The lobby treats it as a black box.
   - Ensure it embeds `Game_St` fields (e.g., `running`) at the start for casting compatibility (see notes in `gameNameAPI.h`).

3. **Configuration Struct**:
   - Define a configs struct (e.g., `<GameName>Configs_St`) for customizable options like FPS.

4. **Error Handling**:
   - Use `Error_Et` for return values (OK, ERROR_NULL_POINTER, etc.).

5. **Function Prototypes**:
   - `Error_Et <gamename>_initGame__full(<GameName>Game_St** game, <GameName>Configs_St configs);` — Allocates and initializes.
   - `void <gamename>_gameLoop(<GameName>Game_St* const game);` — Handles input, update, render.
   - `void <gamename>_freeGame(<GameName>Game_St** game);` — Frees resources.
   - Optional macro: `#define <gamename>_initGame(game, ...) <gamename>_initGame__full(game, (<GameName>Configs_St) { __VA_ARGS__ })` for simpler calls.

6. **Guards and Docs**:
   - Use include guards: `#ifndef <GAMENAME>_API_H`.
   - Add Doxygen comments for functions and file.

### Example: `include/tetrisAPI.h`

```c
#ifndef TETRIS_API_H
#define TETRIS_API_H

/**
 * @file tetrisAPI.h
 * @author Fshimi Hawlk
 * @date 2026-01-07
 * @brief Tetris API with the dedicated *opaque* game structure and function prototypes.
 */

#include "generalAPI.h"

/* Forward declaration — internal definition is private to the module */
typedef struct TetrisGame_St TetrisGame_St;

/**
 * @brief Configuration options passed to the initialization function.
 *
 * Default values should be sensible for standalone and lobby usage.
 */
typedef struct {
    unsigned int fps;          ///< Target frame rate (0 = uncapped, usually 60 or 120)
    // Add more options here when needed (difficulty, audio volume, window flags, ...)
} TetrisConfigs_St;

/**
 * @brief Convenience macro for C99 compound literal initialization syntax.
 *
 * Allows calls like:
 *   tetris_initGame(&game, .fps = 144);
 */
#define tetris_initGame(game, ...) \
    tetris_initGame__full(game, (TetrisConfigs_St){ __VA_ARGS__ })

/* ────────────────────────────────────────────────────────────────────────────
   Core game lifecycle functions
   All functions follow the naming pattern: tetris_*
   ──────────────────────────────────────────────────────────────────────────── */

/**
 * @brief Allocates and initializes a new Tetris game instance.
 *
 * @param[out] game         Double pointer to receive the allocated game handle.
 *                          Set to NULL on failure.
 * @param[in]  configs      Configuration values (FPS, difficulty, etc.)
 *
 * @return OK on success
 * @return ERROR_ALLOC if memory allocation failed
 * @return other Error_Et codes on other initialization failures
 *
 * @pre  *game == NULL
 * @post On success: *game points to a valid, initialized game object
 * @post On failure: *game remains NULL
 *
 * @note Does **not** initialize or manage the Raylib window/context.
 *       Window creation/cleanup is the responsibility of the caller (lobby).
 */
Error_Et tetris_initGame__full(TetrisGame_St** game, TetrisConfigs_St configs);

/**
 * @brief Executes one frame of the game (input → update → render).
 *
 * Should be called once per frame inside the main application loop.
 *
 * @param[in,out] game      Valid game instance (must not be NULL)
 *
 * @pre  game != NULL and game was successfully initialized
 * @pre  Raylib window and drawing context are active
 *
 * @note If the game decides to end (win/lose/quit), it should set
 *       ((Game_St*)game)->running = false;
 */
void tetris_gameLoop(TetrisGame_St* const game);

/**
 * @brief Releases all resources associated with the game and frees the handle.
 *
 * @param[in,out] game      Pointer to the game handle. Will be set to NULL.
 *
 * @pre  game may be NULL or point to a valid initialized game
 * @post *game == NULL
 * @post All game-allocated resources are freed
 *
 * @note Safe to call multiple times (idempotent).
 * @note Does **not** close the Raylib window or terminate the program.
 */
void tetris_freeGame(TetrisGame_St** game);

#endif // TETRIS_API_H
```

**Note**:
- Replace "GameName" / "gameName" with your game's name (e.g., "tetris" / "Tetris").
- Keep signatures consistent across games for lobby compatibility.

## Step 2: Implement the API Functions (`src/<gamename>API.c`)

Create an implementation file for the API. This is where you move your game's core logic.

### Key Elements

1. **Full Struct Definition**:
   - Define the full `struct <GameName>Game_St` here (not in the header).
   - Embed general fields like `bool running;` at the start.
   - Add game-specific fields (e.g., board, score).

2. **Initialization (`<gamename>_initGame__full`)**:
   - Allocate the struct with `calloc(1, sizeof(*game))`.
   - Set defaults (e.g., `game->running = true;`).
   - Apply configs (e.g., `SetTargetFPS(configs.fps)` if non-default).
   - Return `OK` on success, error codes on failure (e.g., `ERROR_ALLOC` if calloc fails).
   - Do **not** manage the Raylib window—lobby handles that.

3. **Game Loop (`<gamename>_gameLoop`)**:
   - Check if `game->running` (early return if not).
   - Handle input (e.g., keys, events).
   - Update game state.
   - Render with `BeginDrawing()` / `EndDrawing()` (use Raylib globals like `ClearBackground`).
   - Set `game->running = false` when game ends (e.g., quit key or game over).

4. **Cleanup (`<gamename>_freeGame`)**:
   - Free any allocated resources inside the struct.
   - `free(*game); *game = NULL;`.
   - Do **not** close the window or exit the process.

5. **Includes and Utils**:
   - Include Raylib, project utils (`userTypes.h`, `logger.h`).
   - Use logger for errors/warnings.

### Example: `src/tetrisAPI.c`

```c
#include "tetrisAPI.h"

#include "raylib.h"
#include "utils/userTypes.h"

#include "logger.h"

// ─────────────────────────────────────────────────────────────────────────────
// Private internal structure definition
// Only visible inside this translation unit
// ─────────────────────────────────────────────────────────────────────────────

struct TetrisGame_St {
    Game_St base;              // Must be first — allows safe cast to Game_St*

    // Game-specific fields
    // int someCounter;
    // Texture2D playerSprite;
    // ... add real fields here
};

// ─────────────────────────────────────────────────────────────────────────────
// Implementation
// ─────────────────────────────────────────────────────────────────────────────

Error_Et tetris_initGame__full(TetrisGame_St** game, TetrisConfigs_St configs) {
    if (game == NULL) {
        LOG_ERROR("NULL game double-pointer passed to init");
        return ERROR_NULL_POINTER;
    }

    *game = calloc(1, sizeof(**game));
    if (*game == NULL) {
        LOG_ERROR("Failed to allocate TetrisGame_St");
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

    LOG_INFO("Tetris initialized successfully (FPS: %u)", configs.fps ? configs.fps : 60);
    return OK;
}

void tetris_gameLoop(TetrisGame_St* const game) {
    if (game == NULL) {
        LOG_ERROR_ONCE("NULL game pointer in gameLoop");
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
    BeginDrawing(); {
        ClearBackground(RAYWHITE);

        // Draw game elements...
        // DrawTexture(game->playerSprite, x, y, WHITE);
    } EndDrawing();

    // Example end condition
    if (IsKeyPressed(KEY_ESCAPE)) {
        game->base.running = false;
    }
}

void tetris_freeGame(TetrisGame_St** game) {
    if (game == NULL || *game == NULL) {
        return;
    }

    // Unload game-specific resources
    // UnloadTexture((*game)->playerSprite);

    free(*game);
    *game = NULL;

    LOG_TRACE("Tetris resources freed");
}
```

**Note**:
- Test standalone first: Update your sub-game's `main.c` to use these functions.
- Ensure no global state leaks—everything in the struct.

## Step 3: Update the Game Code to Use the API

1. **Refactor Existing Code**:
   - Move all game logic into the API functions.
   - Remove window management from game code (lobby handles `InitWindow`, `CloseWindow`).
   - Use opaque pointers if needed for sub-components.

2. **Build as Library**:
   - Update sub-game Makefile: Add `static-lib` target (as per root Makefile integration).
   - Test: `make static-lib` produces `build/lib/lib<gamename>.a`.

3. **Standalone Testing**:
   - Keep a temporary `main.c` in the sub-game for testing: Call init/loop/free in a loop.
   - Once integrated, remove or comment it out.

## Step 4: Integrate into the Lobby (`lobby/src/main.c`)

The lobby uses a scene switcher to launch games.

1. **Include the API**:
   - `#include "../firstparty/APIs/<gamename>API.h"`

2. **Declare Game Pointer**:
   - `<GameName>Game_St* gameNameGame = NULL;`

3. **Scene Switch Logic**:
   - In main loop, use an enum (`GameScene_Et`) for scenes.
   - On trigger (e.g., collision with hitbox): Set scene to game, flag init needed.
   - In game case: If init needed, call `<gamename>_initGame(&gameNameGame, configs);` handle errors.
   - Call `<gamename>_gameLoop(gameNameGame);`
   - If `!((Game_St*)gameNameGame)->running`, free and switch back to lobby.

4. **Error Handling**:
   - On init error, log and switch back to lobby.

### Example from `main.c`

```c
/**
 * @file main.c
 * @author Fshimi Hawlk
 * @date 2026-02-08
 * @brief Program entry point – lobby main loop and game scene manager.
 *
 * This file contains the top-level application loop.
 * It initializes the window and shared resources, runs the lobby,
 * and switches to individual games when triggered (e.g. collision with zone).
 *
 * Games are loaded on demand via their API (e.g. gameNameAPI.h) and run
 * in the same process/window. No separate executables are spawned.
 */

#include "core/game.h"              // GameScene_Et, general game types
#include "ui/app.h"                 // UI helpers (skin menu, buttons, etc.)
#include "ui/game.h"                // Player drawing, platform logic

#include "utils/common.h"           // Common macros and helpers
#include "utils/globals.h"          // Global constants (WINDOW_WIDTH, etc.)

#include "APIs/gameNameAPI.h"       // Example game API – replace/add others

// ─────────────────────────────────────────────────────────────────────────────
// Global / shared state
// ─────────────────────────────────────────────────────────────────────────────


/** Hitbox that triggers the GameName mini-game when player collides */
static Rectangle gameNameHitbox = {
    .x      = 600,
    .y      = -150,
    .width  = 75,
    .height = 75
};

/** Current active scene (lobby or one of the mini-games) */
static GameScene_Et currentScene = GAME_SCENE_LOBBY;

/** Flag: game needs initialization on next frame */
static bool needGameInit = false;

/** Prevents multiple rapid triggers when standing on hitbox */
static bool gameHitGracePeriodActive = false;

/** Example instance of one game – add more for other games */
static GameNameGame_St* gameNameGame = NULL;

// ─────────────────────────────────────────────────────────────────────────────
// Helper functions
// ─────────────────────────────────────────────────────────────────────────────

/**
 * @brief Checks if a game instance is still running.
 * @param game  Pointer to a Game_St-compatible structure
 * @return true if game->running is true, false otherwise
 */
bool isGameRunning(const Game_St* game) {
    return game != NULL && game->running;
}

/**
 * @brief Updates and renders the lobby scene (one frame).
 * @param dt  Frame time delta (from GetFrameTime())
 *
 * Handles player movement, camera, UI menus, and game zone collision detection.
 */
static void lobby_gameLoop(float dt) {
    updatePlayer(&player, platforms, platformCount, dt);
    cam.target = player.position;

    toggleSkinMenu();

    if (isTextureMenuOpen) {
        choosePlayerTexture(&player);
    }

    // Collision check with game zone (gameName example)
    if (CheckCollisionCircleRec(player.position, player.radius, gameNameHitbox)) {
        if (!gameHitGracePeriodActive) {
            currentScene = GAME_SCENE_GAME_NAME;
            needGameInit = true;
            gameHitGracePeriodActive = true;
        }
    } else if (gameHitGracePeriodActive) {
        gameHitGracePeriodActive = false;
    }

    BeginDrawing(); {
        ClearBackground(RAYWHITE);

        BeginMode2D(cam);
        {
            DrawCircle(0, 0, 10, RED);          // Debug origin marker
            drawPlayer(&player);
            drawPlatforms(platforms, platformCount);
            DrawRectangleRec(gameNameHitbox, RED); // Debug hitbox
        }
        EndMode2D();

        DrawText("Multi-Mini-Games", WINDOW_WIDTH / 2 - MeasureText("Multi-Mini-Games", 20) / 2,
                 20, 20, PURPLE);

        drawSkinButton();

        if (isTextureMenuOpen) {
            drawMenuTextures();
        }
    } EndDrawing();
}

// ─────────────────────────────────────────────────────────────────────────────
// Program entry point
// ─────────────────────────────────────────────────────────────────────────────

int main(void) {
    SetTraceLogLevel(LOG_WARNING);

    // ── Initialization ───────────────────────────────────────────────────────
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE);

    // Load shared UI textures
    logoSkinButton = LoadTexture(IMAGES_PATH "logoSkin.png");

    playerTextures[0] = LoadTexture(IMAGES_PATH "earth.png"); playerTextureCount++;
    playerTextures[1] = LoadTexture(IMAGES_PATH "trollFace.png"); playerTextureCount++;

    cam.target = player.position;

    // ── Main loop ────────────────────────────────────────────────────────────
    Error_Et error = OK;

    while (!WindowShouldClose()) {
        float dt = GetFrameTime();

        switch (currentScene) {
            case GAME_SCENE_LOBBY: {
                lobby_gameLoop(dt);
            } break;

            case GAME_SCENE_GAME_NAME: {
                if (needGameInit) {
                    error = gameName_initGame(&gameNameGame, .fps = 120);

                    needGameInit = false;

                    if (error != OK) {
                        log_fatal("GameName initialization failed: error %d", error);
                        gameName_freeGame(&gameNameGame);
                        currentScene = GAME_SCENE_LOBBY;
                    }
                } else {
                    gameName_gameLoop(gameNameGame);
                    if (!isGameRunning((Game_St*)gameNameGame)) {
                        gameName_freeGame(&gameNameGame);
                        currentScene = GAME_SCENE_LOBBY;
                    }
                }
            } break;

            default:
                log_error("Invalid GameScene_Et value: %d", currentScene);
                break;
        }
    }

    // ── Cleanup ──────────────────────────────────────────────────────────────
    if (gameNameGame != NULL) {
        gameName_freeGame(&gameNameGame);
    }

    for (int i = 0; i < playerTextureCount; i++) {
        UnloadTexture(playerTextures[i]);
    }

    UnloadTexture(logoSkinButton);

    CloseWindow();

    return 0;
}

#define LOGGER_IMPLEMENTATION
#include "logger.h"
```

**Note**:
- Cast to `Game_St*` for general fields like `running`.
- Add grace periods for hitboxes to prevent rapid toggling.

## Step 5: Build and Test Integration

1. **Root Build**:
   - `make rebuild-libs`: Builds libs, copies APIs.
   - `make rebuild-exe`: Forces lobby relink.
   - `make run-exe`: Test.

2. **Standalone Sub-Game**:
   - In game folder: `make rebuild run-main` (use temporary main if needed).

3. **Debugging**:
   - `make MODE=clang-debug rebuild-exe run-exe`
   - Check logs for errors.

## Common Pitfalls and Tips

- **Incomplete Types**: Ensure lobby only uses opaque pointers—no direct struct access.
- **Window Management**: Games must not call `InitWindow` / `CloseWindow`.
- **Resource Leaks**: Free everything in `freeGame` (textures, etc.).
- **Configs**: Use defaults; lobby can override (e.g., FPS).
- **Logging**: Use `logger.h` for consistency.
- **Testing**: Add API tests in `tests/`.
- **If Stuck**: Check `makefile.md` for build details; discuss in group chat.