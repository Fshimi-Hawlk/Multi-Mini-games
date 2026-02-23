# Lobby

**Central Hub for Multi Mini-Games Collection**

## Overview

The lobby is the main entry point for the Multi Mini-Games collection. Players control a character that can move around and enter different mini-games by colliding with their portals.

### Core Features

- **Player Movement**: WASD/Arrow keys with double jump capability
- **Skin Selection**: Choose your player appearance from available textures
- **Game Portals**: Visual rectangles representing available mini-games
- **Graceful Degradation**: Lobby launches even if some mini-game APIs are unavailable

### Controls

- **WASD / Arrow Keys**: Move left/right, jump
- **Double Jump**: Press jump again while in the air
- **Enter Game**: Walk into a game portal

## Available Games

| Portal Color | Game | Status |
|--------------|------|--------|
| Blue | Solitaire | ✅ Available |

## Tech / Structure Notes

- **Language**: C (C99)
- **Graphics**: Raylib 5.5
- **Architecture**: Modular mini-game loading system
- **Folders**:
  - `src/core/`: Game logic
  - `src/ui/`: Rendering and UI
  - `src/utils/`: Helpers, globals, configs
  - `include/`: Public headers
  - `assets/`: Player textures and UI elements

## Building

```bash
cd lobby
make            # Build executable
make run-main   # Build and run
make rebuild    # Clean and rebuild
```

## Adding a New Mini-Game

1. Ensure the game has a proper API in `firstparty/APIs/gameNameAPI.h`
2. Add the API include at the top of `src/main.c`
3. Add entry to the `miniGames[]` array:

```c
{
    .name = "GAME_NAME",
    .hitbox = {x, y, width, height},
    .color = SOME_COLOR,
    .scene = GAME_SCENE_GAMENAME,
    .gameInstance = NULL,
    .available = false,
    .initFunc = (void*(*)(const void*))gameName_initGame,
    .loopFunc = (void(*)(void*))gameName_gameLoop,
    .freeFunc = (void(*)(void*))gameName_freeGame,
    .isRunningFunc = (bool(*)(const void*))gameName_isRunning
}
```

4. Add the corresponding enum value to `GameScene_Et`
5. Add a case in the main loop's switch statement

The lobby will automatically test each game's API at startup and only show available games.
