# Lobby

## Overview

The Lobby is the central hub of the Multi Mini-Games project. It provides a platformer-style menu world where players can navigate to different mini-games by walking to doors or interaction zones.

### Core Gameplay

- Navigate a 2D platformer environment
- Interact with game zones to launch mini-games
- Seamless transition between lobby and games (same window, no executable switching)
- Skin selection and customization

### Technical Features

- Physics-based player movement
- Collision detection with platforms and game zones
- Scene management for multiple mini-games
- Shared resource loading (fonts, textures)

## Project Goals

- Provide an intuitive and fun way to browse available games
- Maintain consistent state between games and lobby
- Support future multiplayer features
- Enable easy addition of new mini-games

## Tech / Structure Notes

- Language: C (C99)
- Graphics: Raylib
- Folders: `src/`, `include/`, `tests/`, `assets/`, `docs/`
- Build system: GNU Make with modular makefiles
- Documentation: Generated via Doxygen -> run `make docs` then see `docs/html/index.html`

## Building

```bash
make              # Release build
make MODE=debug   # Debug build
make run-main     # Run the lobby
make docs         # Generate documentation
```

## Credits

**Last updated: February 24, 2026**  
**Author: [Fshimi Hawlk](https://github.com/Fshimi-Hawlk)**
