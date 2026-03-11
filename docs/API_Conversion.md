# API Conversion & Integration

Converting a Sub-Game to an API and Integrating into the Lobby

**Based on project structure as of Multi Mini-Games monorepo**

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
- Familiarity with the root Makefile: it builds libraries lazily and copies `<gameName>API.h` to `firstparty/APIs/`.
- Lobby code (`lobby/main.c`) uses a scene switcher (e.g., `currentScene`) to toggle between lobby and games.

If your game doesn't meet these, start by aligning it with the template.

## Step 1: Define the Game API Header (`include/<gameName>API.h`)

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
    - `Error_Et <gameName>_initGame__full(<GameName>Game_St** game, <GameName>Configs_St configs);` - Allocates and initializes.
    - `void <gameName>_gameLoop(<GameName>Game_St* const game);` - Handles input, update, render.
    - `void <gameName>_freeGame(<GameName>Game_St** game);` - Frees resources.
    - Optional macro: `#define <gameName>_initGame(game, ...) <gameName>_initGame__full(game, (<GameName>Configs_St) { __VA_ARGS__ })` for simpler calls.

6. **Guards and Docs**:
    - Use include guards: `#ifndef <GAME_NAME>_API_H`.
    - Add Doxygen comments for functions and file.
        Note: Refer to `CodeStyleAndConvetions.md` for more info.

### For an example check out [`gameNameAPI.h`](../firstparty/APIs/gameNameAPI.h)

**Note**:
- Replace "GameName" / "gameName" with your game's name (e.g., "tetris" / "Tetris").
- Keep signatures consistent across games for lobby compatibility.

## Step 2: Implement the API Functions (`<game-name>/src/<gameName>API.c`)

Create an implementation file for the API. This is where you move your game's core logic.

### Key Elements

1. **Full Struct Definition**:
    - Define the full `struct <GameName>Game_St` here (not in the header).
    - Embed general fields like `bool running;` at the start.
    - Add game-specific fields (e.g., board, score).

2. **Initialization (`<gameName>_initGame__full`)**:
    - Allocate the struct with `calloc(1, sizeof(*game))`.
    - Set defaults (e.g., `game->running = true;`).
    - Apply configs (e.g., `SetTargetFPS(configs.fps)` if non-default).
    - Return `OK` on success, error codes on failure (e.g., `ERROR_ALLOC` if calloc fails).
    - Do **not** manage the Raylib window-lobby handles that.

3. **Game Loop (`<gameName>_gameLoop`)**:
    - Check if `game->base.running` (early return if not).
    - Handle input (e.g., keys, events).
    - Update game state.
    - Render with `BeginDrawing()` / `EndDrawing()` (use Raylib globals like `ClearBackground`).
    - Set `game->base.running = false` when game ends (e.g., quit key or game over).

4. **Cleanup (`<gameName>_freeGame`)**:
    - Free any allocated resources inside the struct.
    - `free(*game); *game = NULL;`.
    - Do **not** close the window or exit the process.

5. **Includes and Utils**:
    - Include Raylib, project utils (`userTypes.h`, `logger.h`).
    - Use logger for errors/warnings.

### For an Example check out [tetrisAPI.c](../tetris/src/tetrisAPI.c)

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
    - Test: `make static-lib` produces `build/lib/lib<gameName>.a`.

3. **Standalone Testing**:
    - Keep a temporary `main.c` in the sub-game for testing: Call init/loop/free in a loop.
    - Once integrated, remove or comment it out.

## Step 4: Integrate into the Lobby

The lobby uses a scene switcher to launch games.

1. **Include the API**:
    - `#include "APIs/<gameName>API.h"`

2. **Scene Switch Logic**:
    - In main loop, use an enum (`GameScene_Et`) for scenes.
    - On trigger (e.g., collision with hitbox): Set scene to game, flag init needed.
    - `Game_St** miniRef = &game->subGameManager.miniGames[GAME_SCENE_TETRIS];`
    - `GameNameGame_St* gameNameRef = (GameNameGame_St*) *miniRef;`
    - If init needed, call `<gameName>_initGame(&gameNameRef, [[.configsName = ...], ]);` and handle errors.
    - Call `<gameName>_gameLoop(gameNameRef);`
    - If `!(*miniRef)->running`, 
        - free using `(*miniRef)->free(miniRef)`
        - then switch back to lobby

### Checkout [`main.c`](../lobby/src/main.c)

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

## Credits

**Created: February 10, 2026**  
**Last updated: March 01, 2026**  
**Author: [Fshimi Hawlk](https://github.com/Fshimi-Hawlk)**