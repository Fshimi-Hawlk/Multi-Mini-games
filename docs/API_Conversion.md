# Converting a Sub-Game to an API and Integrating into the Lobby

**Last updated: February 20, 2026**  
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

Create or update a public API header in your game's `include/` folder. This header exposes only what the lobby needs: opaque types, enums, and function prototypes. Do not expose internal structs or details-use forward declarations for opacity.

### Key Elements

1. **Include General API**:
    - Use `#include "APIs/generalAPI.h"` for shared types like `Error_Et` and `Game_St`.

2. **Opaque Game Structure**:
    - Declare `typedef struct <GameName>Game_St <GameName>Game_St;` (forward declaration).
    - This hides internals. The lobby treats it as a black box.
    - Ensure it embeds `Game_St` fields (e.g., `running`) at the start for casting compatibility (see notes in `gameNameAPI.h`).

3. **Configuration Struct**:
    - Define a configs struct (e.g., `<GameName>Configs_St`) for customizable options like FPS.

4. **Error Handling**:
    - Use `Error_Et` for return values (OK, ERROR_NULL_POINTER, etc.).

5. **Function Prototypes**:
    - `Error_Et <gamename>_initGame__full(<GameName>Game_St** game, <GameName>Configs_St configs);` - Allocates and initializes.
    - `void <gamename>_gameLoop(<GameName>Game_St* const game);` - Handles input, update, render.
    - `void <gamename>_freeGame(<GameName>Game_St** game);` - Frees resources.
    - Optional macro: `#define <gamename>_initGame(game, ...) <gamename>_initGame__full(game, (<GameName>Configs_St) { __VA_ARGS__ })` for simpler calls.

6. **Guards and Docs**:
    - Use include guards: `#ifndef <GAMENAME>_API_H`.
    - Add Doxygen comments for functions and file.
        Note: Refer to `CodeStyleAndConvetions.md` for more info.

### Example: `include/tetrisAPI.h`

```c
/**
 * @file tetrisAPI.h
 * @author Fshimi-Hawlk
 * @date 2026-02-07
 * @date 2026-02-18
 * @brief Public API for the Tetris mini-game.
 *
 * Defines the opaque game handle and the minimal lifecycle functions
 * required to integrate Tetris into the lobby.
 *
 * @note The internal structure `TetrisGame_St` is **opaque** outside this module.
 *       Direct field access from the lobby or other modules is forbidden.
 *
 * @see generalAPI.h for the required `Game_St` base structure and `Error_Et` codes
 */

#ifndef TETRIS_API_H
#define TETRIS_API_H

#include "APIs/generalAPI.h"

// ────────────────────────────────────────────────────────────────────────────
// Types
// ────────────────────────────────────────────────────────────────────────────

/**
 * @brief Opaque forward declaration - internal definition is private.
 */
typedef struct TetrisGame_St TetrisGame_St;

/**
 * @brief Configuration parameters for Tetris initialization.
 *
 * Fields have safe defaults when zero-initialized.
 */
typedef struct {
    void __useless;
    unsigned int fps;           ///< Target frame rate (0 = uncapped; common values: 60, 120, 144)
    // Future: difficulty preset, hold piece enabled, ghost piece visibility, etc.
} TetrisConfigs_St;

/**
 * @brief Convenience macro using C99 compound literal syntax.
 *
 * Example usage:
 *   TetrisGame_St* game = NULL;
 *   tetris_initGame(&game, .fps = 120);
 */
#define tetris_initGame(game, ...) \
    tetris_initGame__full((game), (TetrisConfigs_St){ .fps = 60, __VA_ARGS__ })

// ────────────────────────────────────────────────────────────────────────────
// Core lifecycle API
// ────────────────────────────────────────────────────────────────────────────

/**
 * @brief Allocates and initializes a new Tetris game instance.
 *
 * @param[out] game     Double pointer receiving the new game handle (set to NULL on failure)
 * @param[in]  configs      Initialization options
 *
 * @return OK on success
 * @return ERROR_ALLOC on memory allocation failure
 * @return other Error_Et codes on initialization failure
 *
 * @pre  *game == NULL
 * @post On success: *game points to a valid game with base.running = true
 * @post On failure: *game remains NULL
 *
 * @note Does **not** create/manage the Raylib window or context - lobby responsibility.
 */
Error_Et tetris_initGame__full(TetrisGame_St** game, TetrisConfigs_St configs);

/**
 * @brief Runs one complete frame: input processing → logic update → rendering.
 *
 * Called once per frame when Tetris is the active mini-game.
 *
 * @param[in,out] game      Valid game instance handle
 *
 * @return OK on success
 * @return ERROR_NULL_POINTER if game is NULL
 *
 * @pre  game != NULL and was successfully initialized
 * @pre  Raylib drawing context is active (BeginDrawing() called)
 *
 * @note When game-over condition is reached, the function sets
 *       `((Game_St*)game)->running = false;`
 */
Error_Et tetris_gameLoop(TetrisGame_St* const game);

/**
 * @brief Frees all resources owned by the game and releases the handle.
 *
 * @param[in,out] game  Pointer to the game handle (set to NULL after cleanup)
 *
 * @return OK on success
 * @return ERROR_NULL_POINTER if *game is invalid (still sets to NULL)
 *
 * @pre  game may be NULL or point to a valid game
 * @post *game == NULL
 * @post All game-owned resources are freed
 *
 * @note Idempotent - safe to call multiple times.
 * @note Does **not** close the Raylib window or call CloseWindow().
 */
Error_Et tetris_freeGame(TetrisGame_St** game);

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
    - Do **not** manage the Raylib window-lobby handles that.

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

### Example: `tetris/src/tetrisAPI.c`

```c
/**
 * @file tetris.c
 * @author Fshimi-Hawlk
 * @date 2026-02-07
 * @date 2026-02-18
 * @brief Implementation of the Tetris mini-game.
 *
 * Contains the concrete `TetrisGame_St` definition and the core logic:
 *   - piece generation and movement
 *   - collision & line clearing
 *   - scoring & speed progression
 *   - high-score persistence
 *   - rendering (board, current/next piece, score info)
 *
 * The game follows classic Tetris rules with:
 *   - 7 tetrominoes (I, O, T, S, Z, J, L)
 *   - increasing speed every 10 lines
 *   - standard scoring (40/100/300/1200 points x level multiplier)
 *   - game over on spawn collision
 *
 * @see tetrisAPI.h for the public interface
 * @see core/board.h, core/shape.h for board and tetromino logic
 * @see ui/board.h, ui/shape.h for rendering helpers
 */

#include "core/board.h"
#include "core/game.h"
#include "core/shape.h"

#include "ui/board.h"
#include "ui/game.h"
#include "ui/shape.h"

#include "utils/configs.h"
#include "utils/types.h"

#include "APIs/generalAPI.h"
#include "APIs/tetrisAPI.h"

/**
 * @brief Concrete Tetris game state - embeds Game_St as first member.
 */
struct TetrisGame_St {
    Game_St base;                   // Must be first - allows safe cast to Game_St*

    board_t board;

    boardShape_st boardShape;       // Current falling piece
    boardShape_st nextBoardShape;   // Preview of next piece

    speed_st speed;                 // Controls automatic drop timing

    int *clearedLines;              // Temporary buffer for cleared line indices
    int clearedLineAmount;          // Total lines cleared (used for difficulty)

    int *rewardedPointsPerClearedLineCount; // Points per 0/1/2/3/4 lines cleared
    int difficultyMultiplier;       // Increases every 10 lines (affects score & speed)

    int highScore;                  // Loaded from file, updated on game over
};

/* ────────────────────────────────────────────────────────────────────────────
   Lifecycle implementation
   ──────────────────────────────────────────────────────────────────────────── */

Error_Et tetris_freeGameWrapper(void* game) {
    return tetris_freeGame((TetrisGame_St**) game);
}

Error_Et tetris_initGame__full(TetrisGame_St** game, TetrisConfigs_St configs) {
    (void)configs; // Currently unused - future: difficulty, etc.

    *game = malloc(sizeof(TetrisGame_St));
    if (*game == NULL) return ERROR_ALLOC;

    TetrisGame_St* gameRef = *game;
    memset(gameRef, 0, sizeof(*gameRef));

    gameRef->base.freeGame = tetris_freeGameWrapper;
    gameRef->base.running  = true;

    gameRef->speed.duration = 1.0f;

    // Scoring table (classic values)
    gameRef->rewardedPointsPerClearedLineCount = calloc(sizeof(*gameRef->rewardedPointsPerClearedLineCount), 5);
    if (gameRef->rewardedPointsPerClearedLineCount == NULL) {
        tetris_freeGame(game);
        return ERROR_ALLOC;
    }

    gameRef->rewardedPointsPerClearedLineCount[0] = 0;
    gameRef->rewardedPointsPerClearedLineCount[1] = 40;
    gameRef->rewardedPointsPerClearedLineCount[2] = 100;
    gameRef->rewardedPointsPerClearedLineCount[3] = 300;
    gameRef->rewardedPointsPerClearedLineCount[4] = 1200;

    // Line clearing buffer
    gameRef->clearedLines = calloc(4, sizeof(int));
    if (!gameRef->clearedLines) {
        tetris_freeGame(game);
        return ERROR_ALLOC;
    }

    randomShape(&gameRef->boardShape);
    randomShape(&gameRef->nextBoardShape);

    readHighScore(&gameRef->highScore);
    initBoard(gameRef->board);

    return OK;
}

Error_Et tetris_gameLoop(TetrisGame_St* const game) {
    if (game == NULL) return ERROR_NULL_POINTER;
    if (!game->base.running) return OK;

    mouvement(game->board, &game->boardShape);
    automaticDrop(&game->speed, &game->boardShape);

    if (isOOB(game->boardShape) || isColliding(game->board, game->boardShape)) {
        game->boardShape.position.y--;
        putShapeInBoard(game->board, game->boardShape);

        game->boardShape = game->nextBoardShape;
        randomShape(&game->nextBoardShape);

        if (isColliding(game->board, game->boardShape)) {
            writeHighScore(game->highScore, game->base.score);
            game->base.running = false;
            return OK;
        }
    }

    int clearedCount = 0;
    handleLineClears(game->board, game->clearedLines, &clearedCount);

    game->clearedLineAmount += clearedCount;
    game->difficultyMultiplier = (int) fminf(29, game->clearedLineAmount / 10.0f);

    // Score = base points × (level + 1)
    if (clearedCount > 0 && clearedCount <= 4) {
        game->base.score += game->rewardedPointsPerClearedLineCount[clearedCount]
                       * (game->difficultyMultiplier + 1);
    }

    // Speed increases every 10 lines (min 0.3 s delay)
    if (game->clearedLineAmount % 10 == 0) {
        game->speed.duration = fmaxf(0.3f, 1.0f - 0.025f * game->difficultyMultiplier);
    }

    BeginDrawing();
        ClearBackground(BACKGROUND_COLOR);
        DrawFPS(10, 10);

        drawBoard(game->board);
        drawPreview(game->board, game->boardShape);
        drawShape(game->boardShape);
        drawNextShape(game->nextBoardShape);
        drawInformations(game->base.score, game->difficultyMultiplier,
                         game->clearedLineAmount, game->highScore);
    EndDrawing();

    return OK;
}

Error_Et tetris_freeGame(TetrisGame_St** game) {
    if (game == NULL || *game == NULL) return ERROR_NULL_POINTER;
    TetrisGame_St* gameRef = *game;

    if (gameRef->clearedLines != NULL) {
        free(gameRef->clearedLines);
        gameRef->clearedLines = NULL;
    }

    if (gameRef->rewardedPointsPerClearedLineCount != NULL) {
        free(gameRef->rewardedPointsPerClearedLineCount);
        gameRef->rewardedPointsPerClearedLineCount = NULL;
    }

    free(gameRef);
    *game = NULL;

    return OK;
}
```

**Note**:
- Test standalone first: Update your sub-game's `main.c` to use these functions.
- Ensure no global state leaks-everything in the struct.

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
    - `#include "APIs/<gamename>API.h"`


2. **Scene Switch Logic**:
    - In main loop, use an enum (`GameScene_Et`) for scenes.
    - On trigger (e.g., collision with hitbox): Set scene to game, flag init needed.
    - `Game_St** miniRef = &game->subGameManager.miniGames[GAME_SCENE_TETRIS];`
    - `GameNameGame_St* gameNameRef = (GameNameGame_St*) *miniRef;`
    - If init needed, call `<gamename>_initGame(&gameNameRef, [[.configsName = ...], ]);` and handle errors.
    - Call `<gamename>_gameLoop(gameNameRef);`
    - If `!(*miniRef)->running`, 
        - free using `(*miniRef)->free(miniRef)`
        - then switch back to lobby

### Example from `main.c`

```c
/**
 * @file main.c
 * @author LeandreB8
 * @date 2026-01-12
 * @date 2026-02-18
 * @brief Program entry point – lobby main loop and game scene manager.
 *
 * Contributors:
 * - LeandreB8:
 *    - Implemented basic lobby's logic (initialization, game loop, ...)
 * - Fshimi-Hawlk:
 *    - Moved & reworked lobby's initialization, game loop and freeing logic in dedicated `lobbyAPI` files
 *    - Implememted sub-game playablity inside lobby logic via API
 *    - Added documentation
 *
 * This file contains the top-level application loop.
 * It initializes the window and shared resources, runs the lobby,
 * and switches to individual games when triggered (e.g. collision with zone).
 *
 * Games are loaded on demand via their API (e.g. tetrisAPI.h) and run
 * in the same process/window. No separate executables are spawned.
 */

#include "lobbyAPI.h"
#include "APIs/tetrisAPI.h"


// ─────────────────────────────────────────────────────────────────────────────
// Program entry point
// ─────────────────────────────────────────────────────────────────────────────

int main(void) {

    // ── Main loop ────────────────────────────────────────────────────────────
    Error_Et error = OK;

    Game_St** miniRef = NULL;
    LobbyGame_St* game = NULL;
    if (lobby_initGame(&game) == ERROR_ALLOC) {
        log_fatal("Couldn't load the lobby properly.");
        return 1;
    }

    while (!WindowShouldClose()) {
        switch (game->subGameManager.currentScene) {
            case GAME_SCENE_LOBBY: {
                lobby_gameLoop(game);
            } break;

            case GAME_SCENE_TETRIS: {
                miniRef = &game->subGameManager.miniGames[GAME_SCENE_TETRIS];
                TetrisGame_St** tetrisRef = (TetrisGame_St**) miniRef;
                if (game->subGameManager.needGameInit) {
                    error = tetris_initGame(tetrisRef); // Note: no configs were given as their are optional the way it was designed
                    game->subGameManager.needGameInit = false;

                    if (error != OK) {
                        log_fatal("Tetris initialization failed: error %d", error);
                        tetris_freeGame(tetrisRef);
                        game->subGameManager.currentScene = GAME_SCENE_LOBBY;
                        break;
                    }
                }

                tetris_gameLoop(*tetrisRef);

                if (!(*miniRef)->running) {
                    tetris_freeGame(tetrisRef);
                    game->subGameManager.currentScene = GAME_SCENE_LOBBY;
                }
            } break;

            default:
                log_error("Invalid GameScene_Et value: %d", game->subGameManager.currentScene);
                break;
        }
    }

    // ── Cleanup ──────────────────────────────────────────────────────────────

    lobby_freeGame(&game);

    return 0;
}

#define LOGGER_IMPLEMENTATION
#include "logger.h"
```

**Note**:
- Creation of `miniRef` avoid to constantly cast to `Game_St*` for general fields accessing like `running`.
- Add grace periods for hitboxes to prevent rapid toggling.

## Step 5: Build and Test Integration

1. **Root Build**:
    - `make rebuild-libs`: Builds libs, copies APIs.
    - `make rebuild-exe`: Forces lobby relink.
    - `make run-exe`: Test.
    - One liner: `make MODE=strict-debug rebuild run-exe`

2. **Standalone Sub-Game**:
    - In game folder: `make rebuild run-main` (use temporary main if needed).

3. **Debugging**:
    - `make MODE=clang-debug rebuild-exe run-exe`
    - Check logs for errors.

## Common Pitfalls and Tips

- **Incomplete Types**: Ensure lobby only uses opaque pointers-no direct struct access.
- **Window Management**: Games must not call `InitWindow` / `CloseWindow`.
- **Resource Leaks**: Free everything in `freeGame` (textures, etc.).
- **Configs**: Use defaults; lobby can override (e.g., FPS).
- **Logging**: Use `logger.h` for consistency.
- **Testing**: Add API tests in `tests/`.
- **If Stuck**: Check `makefile.md` for build details; discuss in group chat.