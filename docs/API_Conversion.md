# API Conversion & Integration

Converting a Sub-Game to an API and Integrating into the Lobby

**Based on project structure as of March 16, 2026 – Multi Mini-Games monorepo**

This guide explains step-by-step how to convert a standalone mini-game (like `tetris/`, or a future `block-blast/`) into a reusable API that the lobby can load and run inside the same window - no new process, no extra window, just smooth switching.

We assume you already have a working standalone game using our Makefile setup. This draws from `generalAPI.h`, the example `gameNameAPI.h` / `.c`, and how `lobby/main.c` works.  
Key ideas we stick to:
- **opaque structures** -> lobby never sees inside your game (good encapsulation)
- standard interface: init, loop, free functions
- errors always returned via `Error_Et`
- game runs as a "scene" inside the lobby main loop

## Prerequisites

- Your game folder follows `sub-project-example/` structure: `src/`, `include/`, `tests/`, `Makefile`
- Standalone build works: `make rebuild run-main` runs without crashing
- Raylib is linked (usually via `-lraylib` in `BASE_LDFLAGS`)
- You know the root Makefile copies game API headers to `firstparty/APIs/` during build
- Lobby uses a scene switcher (`currentScene` in `lobby/main.c`) to go between lobby and games

If any of that is missing, fix it first by copying the template and getting standalone mode working.

**Quick reminder**: After building from root, your `<gameName>API.h` gets copied to `firstparty/APIs/`. That's where the lobby includes it from (`#include "APIs/<gameName>API.h"`). Check that folder if you're lost.

## Step 1: Define the Game API Header (`include/<gameName>API.h`)

Create or update the public header in your game's `include/` folder.  
This file only shows what the lobby needs - opaque types, function prototypes, maybe configs. Nothing internal.

### Key Elements

1. **Include shared base API**  
   `#include "APIs/generalAPI.h"` -> gives `Error_Et`, `Game_St`, etc.

2. **Opaque game struct**  
   `typedef struct <GameName>Game_St <GameName>Game_St;` (forward declaration only)  
   Lobby sees it as a black box.  
   **Important**: in the .c file, make `Game_St base;` the very first member so casting works safely.

3. **Configs struct (optional but nice)**  
   `typedef struct { int fps; /* ... */ } <GameName>Configs_St;`

4. **Error handling**  
   All init-like functions return `Error_Et` (OK, ERROR_ALLOC, etc.)

5. **Main function prototypes**  
   - `Error_Et <gameName>_initGame__full(<GameName>Game_St** game, <GameName>Configs_St configs);`  
     Allocates + inits  
   - `void <gameName>_gameLoop(<GameName>Game_St* const game);`  
     Input, update, render one frame  
   - `void <gameName>_freeGame(<GameName>Game_St** game);`  
     Cleanup everything  
   - Optional helper macro for easy init:  
     `#define <gameName>_initGame(game, ...) <gameName>_initGame__full(game, (<GameName>Configs_St){ __VA_ARGS__ })`

6. **Guards + docs**  
   `#ifndef <GAME_NAME>_API_H`  
   Add Doxygen-style comments (see `CodeStyleAndConventions.md`)

### Example

Look at [`firstparty/APIs/gameNameAPI.h`](../firstparty/APIs/gameNameAPI.h) or the real `tetrisAPI.h`

**Note**  
Replace `<GameName>` / `<gameName>` with your game (Tetris / tetris, etc.).  
Keep function names and signatures the same across all games so lobby code stays simple.

## Step 2: Implement the API (`src/<gameName>API.c`)

This is where you put the real struct definition and the function bodies.

### Key Elements

1. **Full struct**  
   Define `struct <GameName>Game_St { Game_St base; /* your fields */ };`  
   `base` **must** be first.

2. **Init function**  
   - `calloc(1, sizeof(*game))`  
   - `game->base.running = true;`  
   - Apply configs (e.g. `if (configs.fps > 0) SetTargetFPS(configs.fps);`)  
   - Return error if alloc fails  
   - **Do NOT** call InitWindow or anything window-related

3. **Game loop**  
   - Early return if `!game->base.running`  
   - Input -> update logic -> `BeginDrawing()` -> render -> `EndDrawing()`  
   - Set `game->base.running = false` when game should quit

4. **Free function**  
   - Free textures, sounds, arrays, etc. inside the struct  
   - `free(*game); *game = NULL;`

5. **Includes**  
   Raylib, `logger.h`, `baseTypes.h`, `configs.h`, etc.

Look at [`tetris/src/tetrisAPI.c`](../tetris/src/tetrisAPI.c) for a full example.

**Note**  
Test standalone first by updating your game's `main.c` to call these functions.  
No global variables - everything lives inside the struct.

## Step 3: Refactor game code to use the API

- Move logic into init / loop / free  
- Remove `InitWindow`, `CloseWindow`, `SetTargetFPS` from game code (lobby does it)  
- Update Makefile to have a `static-lib` target  
- Test `make static-lib` -> should give `build/lib/lib<gameName>.a`

Keep a temporary standalone `main.c` for testing if you want.

## Step 4: Plug it into the lobby

In `lobby/src/main.c`:

1. `#include "APIs/<gameName>API.h"`

2. Add to `GameScene_Et` enum

3. In main loop, when player hits trigger:
   - Get pointer: `Game_St** miniRef = &game->subGameManager.miniGames[GAME_SCENE_XXX];`
   - Cast if needed: `<GameName>Game_St* ref = (<GameName>Game_St*)*miniRef;`
   - If first time: call init + error check
   - Call loop: `<gameName>_gameLoop(ref);`
   - If `!(*miniRef)->running`: call free, switch scene back to lobby

See how tetris is integrated in [`lobby/src/main.c`](../lobby/src/main.c)

**Tip**: Add a short grace/cooldown period after leaving a game so you don't re-enter instantly.

## Step 5: Build and test everything

Root folder:
- `make rebuild-libs` -> builds libs + copies APIs to firstparty/APIs/
- `make rebuild-exe` -> relink lobby
- `make run-exe`
- Handy one-liner: `make MODE=strict-debug rebuild run-exe`

Game folder (standalone):
- `make rebuild run-main`

With sanitizers:
- `make MODE=clang-debug rebuild-exe run-exe`

Check logs if something breaks.

## Common Pitfalls and Tips

- Forgot to put `Game_St base` as first struct member -> casting breaks  
- Game calls `InitWindow` / `CloseWindow` -> crash or weird behaviour  
- Leaking resources -> free textures/sounds/allocs in freeGame  
- No `running` check at loop start -> runs after quit  
- Rapid re-trigger on exit -> add 0.3–0.5s cooldown  
- Configs ignored -> lobby can override, but use defaults  
- Logging missing -> use logger.h macros  
- No tests -> add some in `tests/` folder  
- Still stuck? Check `makefile.md`, look at tetris files, or ask in group chat

## Credits

**Created:** February 10, 2026  
**Last updated:** March 17, 2026  
**Authors:**
- [Fshimi Hawlk](https://github.com/Fshimi-Hawlk)