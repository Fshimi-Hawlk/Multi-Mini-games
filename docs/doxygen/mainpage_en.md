@page Home Page

# Multi Mini-Games

This monorepo hosts a collection of small, independent mini-games built in C with raylib as the primary graphics and input library.  
The core concept is to unify these games under a single executable through a central **lobby** - a basic 2D platformer hub where players can explore a simple world, interact with elements, and trigger mini-games via collision zones. Games run in-place without spawning new processes or windows, ensuring a seamless experience.

As of late February 2026, the project includes:
- A functional lobby with player movement, physics, and UI elements like skin selection.
- One integrated mini-game: a classic **Tetris** implementation.
- Shared infrastructure for logging, types, utilities, and API integration.
- Build tools and documentation to facilitate adding more games.

The setup emphasizes modularity, with games developed on branches before merging, and a focus on clean, consistent code to ease collaboration.

## Project Goals and Vision

- **Mini-Game Variety**: Create diverse, fun games that stand alone but integrate well (e.g., puzzles, arcade, memory-based).
- **Unified Experience**: Use the lobby as a playful menu system - no traditional UI lists; instead, physical navigation adds engagement.
- **Technical Focus**: Stick to C for performance and learning; leverage raylib for cross-platform simplicity without heavy dependencies.
- **Extensibility**: Design with future features in mind, like local saves, leaderboards, or basic multiplayer.
- **Collaboration-Friendly**: Monorepo with clear workflows, style guides, and templates to onboard contributors quickly.

Potential expansions (see TODO.md for priorities):
- More games: Snake, Block-Blast, Solitaire, etc.
- Persistent data: High-scores, unlocked skins, settings.
- Polish: Audio configs, fullscreen toggles, error recovery.

## Getting Started: Build and Run

The Makefile supports incremental builds, multiple modes (release/debug/sanitizers), and targets for libs, executables, and tests.

Modes include:
- `release` (optimized)
- `debug` (symbols + warnings)
- `strict-debug` (warnings as errors)
- `clang-debug` (strict with sanitizers)
- `valgrind-debug` (strict with memory checks at runtime).  

### Start from the root directory.  

Common commands:
```bash
# Build static libs from merged games + link lobby executable (incremental)
make libs
make bin

# Run the lobby
make run-exe

# Full rebuild in debug mode with strict warnings
make MODE=strict-debug rebuild

# Rebuild + run with address/undefined sanitizers (clang required)
make MODE=clang-debug rebuild run-exe

# Force relink lobby only (after lib changes)
make rebuild-exe

# Build and run all tests (lobby + games)
make tests
make run-tests

# Clean everything (root + sub-projects)
make clean-all

# Full command list
make help
```

Detailed root Makefile guide: @ref root_makefile "Root Makefile documentation"

### Inside a sub-project folder (e.g., tetris/ or a new game):

```bash
# Standalone build + run
make rebuild run-main

# Tests
make run-tests MODE=clang-debug

# Clean
make clean
```

Sub-project Makefile details: @ref sub_makefile "Sub-Project Makefile documentation"

Internal Makefile structure (for advanced tweaks): @ref makefile_structure "Makefile System Structure"

## Root-Level Repository Structure

The monorepo uses a flat layout with shared folders at the top and per-game subfolders.

```
.
├── assets/                     # Images, sounds, fonts - shared or per game
│   └── fonts/                  # Different fonts used by the games
├── docs/                       # Aggregated/shared documentation across all games/lobby
│   ├── API_Conversion.md       # Explains how to convert a game into an API to be intergrated into the lobby
│   └── makefile.md             # Explains make commands to build the entire project
├── LICENSE                     # Project license
├── CHANGELOG.md                # High-level changes (branch creations, merges, shared updates)
├── CONTRIBUTING.md             # Internal guidelines
├── TODO.md                     # Internal reminders (lobby integration, docs, etc.)
├── .gitignore                  # Ignores build/, logs/, docs/, etc.
├── Makefile                    # Root-level build: lobby + all merged games
├── README.md                   # <-- This file
├── sub-project-example/        # Template structure when starting a new game
├── lobby/                      # The lobby follows the same structure as sub-project-example
├── firstparty/                 # Reusable single-header libs made by the team
└── thirdparty/                 # External libraries and headers
    └── NOTICE                  # Third-party license notices & attributions
```

## Sub-Project-Level Repository structure

```
sub-project/
├── assets/                 # Sub-project-specific images, sounds, etc.
├── docs/                   # generated documentation
├── include/                # .h files
│   ├── core/               # Core logic of the game
│   ├── ui/                 # Draw/Render of the game/app UI
│   ├── setups/             # Init and free of the diverse game/app elements
│   ├── utils/              # Utils like common.h, globals.h, configs.h, ...
│   └── subProjectAPI.h     # API header to be used by the lobby
├── src/                    # .c files -> same structure as include + main
├── tests/                  # unit tests
├── CHANGELOG.md            # Per-sub-project detailed changelog
├── Makefile                # Makefile to build the sub-project
├── make/                   # Split of the Makefile into distinct logical parts
├── makefile.md             # Explains make commands to build the sub-project
├── Doxyfile.min            # Minimal Doxygen configs for per-sub-project docs
└── README.md               # Template README for new sub-projects
```

After merging the first game (e.g. block-blast):

```
├── block-blast/              # Merged game – same internal structure as sub-project
```

## Key Modules and Components

### Lobby Module
Serves as the main entry point and scene manager.

- **Physics and Movement**: Basic platformer with gravity, jumps, coyote time, friction (tunable in configs.h).
- **UI Elements**: Skin toggle button, texture menu, debug hitboxes.
- **Game Switching**: Detects player collision with game hitboxes, loads via API (e.g., tetris_initGame), runs loop until running=false.
- **Configs**: Handles video/audio settings via systemSettings.h.

Core files: lobby/src/main.c (entry), lobbyAPI.h/c (API), userTypes.h (structs/enums like Player_st, GameScene_Et).

See: @ref lobby "Lobby Module Overview"

### Tetris Module
A faithful classic Tetris clone.

- **Gameplay**: Random tetrominoes, rotation/move/drop, collision, line clears.
- **Progression**: Score based on clears (multiplied by level), speed ramps up.
- **UI**: Board render, next piece preview, score/level display.
- **Integration**: Opaque TetrisGame_St, init/loop/free functions.

Core files: tetrisAPI.h/c (API), core/board.h (logic), ui/board.h (render).

See: @ref tetris "Tetris Module Overview"

### Shared Utilities
Reusable across all parts.

- **Logging**: logger.h - levels (info/warn/error), colors, file output in debug, stack traces on Linux.
- **Types**: baseTypes.h - fixed-width (u8/s32), vectors (u32Vector2), shorthands (uint/iVector2).
- **Configs**: configs.h - ASSET_PATH, WINDOW_WIDTH, GRAVITY, JUMP_FORCE, etc.
- **Utils**: utils.h - macros (clamp, swap, UNREACHABLE), functions (randint, getTextureRec).
- **API Base**: generalAPI.h - Error_Et (OK, ERROR_ALLOC, etc.), BaseGame_St (running, score, freeGame).
- **System Settings**: systemSettings.h - VideoSettings_St/AudioSettings_St, apply functions.

Globals like windowRect, appFont in globals.h.

## Development Workflow

- **New Game**: Copy sub-project-example/, work on branch, merge when ready.
- **API Conversion**: Make game lobby-compatible - opaque struct, embed BaseGame_St, provide init/loop/free.
- **Commits**: Use prefixes (feat:, fix:, docs:, refactor:, etc.) - see CONTRIBUTING.md.
- **Reviews**: Run tests/sanitizers before PR; ping group for feedback.
- **Docs**: Update CHANGELOG.md for big changes; use Doxygen comments in code.

Full guide: @ref contributing "CONTRIBUTING.md"

Style rules: @ref code_style "CodeStyleAndConventions.md"

Integration steps: @ref api_conversion "API_Conversion.md"

## Testing and Debugging

- **Modes**: Use clang-debug for sanitizers, valgrind-debug for leaks.
- **Tests**: Per-module (tests/ folders), run via make run-tests - logs in timestamped folders.
- **Logging**: VERBOSE=1 for build commands; log_* macros for runtime.
- **Tools**: make run-gdb for debugging main.

## Roadmap and Open Tasks

Short-term: Integrate more games, add saves/leaderboards.  
Long-term: Multiplayer modes, online features?  

Details: @ref todo "TODO.md"

## License

zlib/libpng - permissive for use/distribution.  

Full: @ref license "LICENSE.md"

Thanks to raylib (Ramon Santamaria), nob.h (Tsoding), and others - credits in headers.

## Credits

**Created:** February 2026  
**Last update:** February 27, 2026  
**Author:** [Fshimi Hawlk](https://github.com/Fshimi-Hawlk)  