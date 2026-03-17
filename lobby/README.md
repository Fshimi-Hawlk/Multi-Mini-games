# Lobby

**Central Hub for Multi-Mini-Games**

## Overview

The Lobby is a 2D platformer-style navigation hub that serves as the main entry point for the Multi-Mini-Games project. Players can explore a platforming environment, customize their character's appearance through a skin selection system, and enter various mini-games through interactive trigger zones.

### Core Gameplay

- **2D Platformer Movement**: Navigate through a world of platforms using WASD or arrow keys
- **Double Jump**: Perform up to two jumps before landing
- **Skin Selection**: Choose from multiple character appearances (press P or click the skin button)
- **Mini-Game Zones**: Enter designated areas to launch mini-games like Bowling

### Controls

| Key | Action |
|-----|--------|
| WASD / Arrow Keys | Movement |
| Space / W / Up | Jump |
| E / Enter | Enter game zone (when nearby) |
| P | Toggle skin menu |
| Mouse Click | Select skin (in menu) |
| 1-4 | Quick skin selection |

### Available Mini-Games

- **Bowling**: 3D bowling simulation (enter the blue zone)

## Project Structure

```
lobby/
├── include/
│   ├── lobbyAPI.h          # Public API for lobby lifecycle
│   ├── utils/
│   │   ├── types.h         # Core type definitions
│   │   ├── configs.h       # Compile-time constants
│   │   ├── common.h        # Common includes
│   │   ├── globals.h       # Global state declarations
│   │   └── utils.h         # Utility macros/functions
│   ├── core/
│   │   └── game.h          # Game logic functions
│   └── ui/
│       ├── game.h          # World-space rendering
│       └── app.h           # Screen-space UI rendering
├── src/
│   ├── lobbyAPI.c          # Main lobby implementation
│   ├── main.c              # Entry point with bowling integration
│   ├── core/
│   │   └── game.c          # Player physics, collision, game zones
│   ├── ui/
│   │   ├── game.c          # Draw player, platforms, zones
│   │   └── app.c           # Draw menus, HUD
│   └── utils/
│       ├── globals.c       # Global state definitions
│       └── utils.c         # Utility functions
├── assets/
│   └── images/             # Textures (earth.png, trollFace.png, etc.)
├── tests/
├── Makefile
└── README.md
```

## Building

```bash
# Debug build (default)
make

# Release build
make MODE=release

# Run with valgrind
make run-main MODE=valgrind-debug

# Clean and rebuild
make rebuild
```

## Integration with Bowling

The lobby integrates with the bowling mini-game through:

1. **Scene Switching**: When the player enters the bowling zone and presses E, the lobby switches to `GAME_SCENE_BOWLING`
2. **State Management**: The `SubGameManager_St` handles initialization flags and game state pointers
3. **Resource Management**: Bowling resources are loaded on-demand and freed when returning to lobby
4. **Window Resizing**: Screen dimensions are adjusted for bowling and restored when returning

### Adding New Mini-Games

1. Add a new `GAME_SCENE_*` entry to `GameScene_Et` enum in `types.h`
2. Add a hitbox rectangle to `lobby_initGame__full()` in `lobbyAPI.c`
3. Add a game zone entry to `gameZones[]` in `globals.c`
4. Add the scene switch case in `main.c`
5. Include the game's API header and call init/gameLoop/free functions

## Tech / Structure Notes

- **Language**: C (C99)
- **Graphics**: Raylib 5.5
- **Folders**: `src/`, `include/`, `tests/`, `assets/`
- **Build System**: Make with multiple build modes
- **Dependencies**: bowling (linked statically)

## Credits

**Last updated: February 25, 2026**  
**Based on template by [Fshimi-Hawlk](https://github.com/Fshimi-Hawlk)**
