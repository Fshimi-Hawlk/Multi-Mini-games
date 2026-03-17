@page api_conversion API Conversion & Integration

@ref index "Back to Home"

**Last checked against project structure:** March 16, 2026  

**Converting a standalone mini-game into a lobby-compatible API**

This page explains the steps to take an independent mini-game (for example the `tetris/` folder) and turn it into something the lobby can load and run directly inside the same window. No spawning new processes, no extra windows — just clean scene switching.

The approach is based on how we integrated Tetris, and follows the patterns in `generalAPI.h`, the example API files, and `lobby/src/main.c`.

**Core principles we follow**
- Use opaque structs so the lobby never accesses game internals
- Provide exactly three main functions: init, loop, free
- Return errors using the shared `Error_Et` enum
- Treat each mini-game as a temporary "scene" the lobby can start and stop

## Prerequisites

Before starting conversion make sure:
- Your game folder matches the `sub-project-example/` layout (`src/`, `include/`, `tests/`, `Makefile`)
- Standalone mode works: `make rebuild run-main` runs the game without errors
- Raylib is linked correctly through the Makefile

If any of these are missing, copy the template folder and get standalone running first.

## Step 1 – Create the public API header

File: `include/<gameName>API.h` (example: `tetrisAPI.h`)

This header is the only thing the lobby will see. Keep it minimal: opaque type, function declarations, optional config struct.

Typical content:

```c
#ifndef TETRIS_API_H
#define TETRIS_API_H

#include "baseTypes.h"
#include "APIs/generalAPI.h"

typedef struct TetrisGame_St TetrisGame_St;

typedef struct {
    int fps;            // target FPS, 0 = use default
    // add other options later if needed
} TetrisConfigs_St;

Error_Et tetris_initGame__full(TetrisGame_St** game, TetrisConfigs_St configs);

#define tetris_initGame(game, ...) \
    tetris_initGame__full(game, (TetrisConfigs_St){ __VA_ARGS__ })

void tetris_gameLoop(TetrisGame_St* const game);

void tetris_freeGame(TetrisGame_St** game);

#endif
```

**Important note**  
In the implementation (.c file), the first member of your struct **must** be `Game_St base;` so the lobby can safely cast to the shared `Game_St*` type.

See the real files in `firstparty/APIs/` or `tetris/include/tetrisAPI.h` for reference.

## Step 2 – Implement the API functions

File: `src/<gameName>API.c`

Here you define the full struct and write the three functions.

Basic skeleton:

```c
struct TetrisGame_St {
    Game_St base;       // MUST be first member
    // your game-specific fields
    // Board board;
    // int score;
    // ...
};

Error_Et tetris_initGame__full(TetrisGame_St** game_out, TetrisConfigs_St configs) {
    TetrisGame_St* game = calloc(1, sizeof(TetrisGame_St));
    if (!game) return ERROR_ALLOC;

    game->base.running = true;
    // initialize your data, load assets, etc.
    // if (configs.fps > 0) SetTargetFPS(configs.fps);

    *game_out = game;
    return OK;
}

void tetris_gameLoop(TetrisGame_St* const game) {
    if (!game->base.running) return;

    // handle input
    // update logic
    BeginDrawing();
        // render everything
    EndDrawing();

    // set running = false when game should end
}

void tetris_freeGame(TetrisGame_St** game) {
    if (!game || !*game) return;
    // free textures, unload sounds, release memory
    free(*game);
    *game = NULL;
}
```

Never call `InitWindow`, `CloseWindow` or manage the window inside these functions — the lobby already did that.

## Step 3 – Refactor your game logic

Move all your game code into the three API functions:
- Initialization goes in `init`
- Per-frame work goes in `loop`
- Cleanup goes in `free`

Remove any window creation/closing calls from your original code.

## Step 4 – Integrate into the lobby

In `lobby/src/main.c`:
- Add `#include "APIs/<gameName>API.h"`
- Add your game to the `GameScene_Et` enum
- When the player collides with the trigger zone:
  - Get the pointer: `Game_St** miniRef = &game->subGameManager.miniGames[GAME_SCENE_YOURGAME];`
  - Cast once: `<GameName>Game_St** ref = (<GameName>Game_St**) miniRef;`
  - If first time: call init and check for errors
  - Call the loop every frame
  - When `!(*miniRef)->running`: call free and switch back to lobby scene

We use `miniRef` to avoid casting every time we check `running`.

See how it's done for Tetris in `lobby/src/main.c`.

## Step 5 – Build and verify

From the repository root:

```bash
# Normal full rebuild + run
make MODE=strict-debug rebuild run-exe

# With sanitizers (good for catching bugs)
make MODE=clang-debug rebuild run-exe

# Or valgrind if you don't have clang
make MODE=valgrind-debug rebuild run-exe
```

From inside your game folder (for standalone checks):

```bash
make MODE=strict-debug rebuild run-main
```

Watch the console logs if something fails.

## Common issues we ran into

- `Game_St base` not first in struct -> casting fails silently
- Game calls `InitWindow` / `CloseWindow` -> double-init crash
- Textures/sounds not freed -> memory leaks
- No early return in loop when `!running` -> game keeps running after quit
- Instant re-trigger when exiting -> add cooldown on hitbox
- Forgetting to copy API header -> lobby include fails

If something breaks, check @ref sub_project_makefile "Sub Project Makefile" page, compare with `tetrisAPI.c`, or ask in the group chat.

**Created:** February 10, 2026  
**Last updated:** March 16, 2026  
**Author:** [Fshimi Hawlk](https://github.com/Fshimi-Hawlk)

@ref index "Back to Home"