@page api_conversion API Conversion & Integration

Converting a Sub-Game to an API and Integrating into the Lobby

Based on project structure as of Multi Mini-Games monorepo.

This guide provides a step-by-step explanation of how to convert an individual sub-game (e.g., `tetris/`, `block-blast/`) into a reusable API that can be integrated into the main lobby. The goal is to allow the lobby to launch and run the game seamlessly within the same window, without restarting the application or managing separate executables.

The process assumes you have a working standalone sub-game built with the project's Makefile system. It draws from the provided API examples (`generalAPI.h`, `gameNameAPI.h`, `gameNameAPI.c`) and lobby code (`main.c`). Key principles:
- Use **opaque** structures to hide internal game details from the lobby (encapsulation).
- Provide a standard interface: init, loop, free functions.
- Handle errors consistently with `Error_Et`.
- Ensure the game can run as a "scene" within the lobby's main loop.

## Prerequisites

- Your sub-game folder follows the template (`sub-project-example/`): `src/`, `include/`, `tests/`, `Makefile`.
- The game runs standalone: `make` rebuild run-main works without errors.
- Raylib is linked via the Makefile (e.g., `-lraylib`).

## Step 1: Create the API Header

Define an opaque handle and the required functions.

Example (`gameNameAPI.h`):

```c
#ifndef GAME_NAME_API_H
#define GAME_NAME_API_H

#include "baseTypes.h"
#include "APIs/generalAPI.h"

typedef struct GameNameGame_St GameNameGame_St;

Error_Et gameName_initGame(GameNameGame_St** game, ...);

Error_Et gameName_gameLoop(GameNameGame_St* const game);

Error_Et gameName_freeGame(GameNameGame_St** game);

bool gameName_isRunning(const GameNameGame_St* game);

#endif
```

**Note**: Embed `BaseGame_St` as the first member in the concrete struct for type-safe casting.

## Step 2: Implement the API

In `gameNameAPI.c`, define the internal struct and functions.

## Step 3: Adapt Game Logic

Remove window management; use lobby's drawing context.

## Step 4: Integrate into Lobby

Update `main.c` to load and run the game via API.

**Note**: Add grace periods for hitboxes to prevent rapid toggling.

## Step 5: Build and Test Integration

1. **Root** Build:
   - `make` rebuild-libs: Builds libs, copies APIs.
   - `make` rebuild-exe: Forces lobby relink.
   - `make` run-exe: Test.
   - One liner: `make` MODE=strict-debug rebuild run-exe

2. **Standalone** Sub-Game:
   - In game folder: `make` rebuild run-main (use temporary main if needed).

3. **Debugging**:
   - `make` MODE=clang-debug rebuild-exe run-exe
   - Check logs for errors.

## Common Pitfalls and Tips

- **Incomplete** Types: Ensure lobby only uses opaque pointers-no direct struct access.
- **Window** Management: Games must not call `InitWindow` / `CloseWindow`.
- **Resource** Leaks: Free everything in `freeGame` (textures, etc.).
- **Configs**: Use defaults; lobby can override (e.g., FPS).
- **Logging**: Use `logger.h` for consistency.
- **Testing**: Add API tests in `tests/`.
- **If** Stuck: Check @ref root_makefile for build details; discuss in group chat.

**Created:** February 10, 2026  
**Last updated:** March 02, 2026  
**Author:** [Fshimi Hawlk](https://github.com/Fshimi-Hawlk)

@ref mainpage "Back to Home"