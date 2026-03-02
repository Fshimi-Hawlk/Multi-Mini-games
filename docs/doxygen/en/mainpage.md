@mainpage Home page

# Multi Mini-Games

This monorepo hosts a collection of small, independent mini-games built in C with raylib as the primary graphics and input library.  
The core concept is to unify these games under a single executable through a central @b lobby - a basic 2D platformer hub where players can explore a simple world, interact with elements, and trigger mini-games via collision zones. Games run in-place without spawning new processes or windows, ensuring a seamless experience.

As of late February 2026, the project includes:
- A functional lobby with player movement, physics, and UI elements like skin selection.
- One integrated mini-game: a classic @b Tetris implementation.
- Shared infrastructure for logging, types, utilities, and API integration.
- Build tools and documentation to facilitate adding more games.

The setup emphasizes modularity, with games developed on branches before merging, and a focus on clean, consistent code to ease collaboration.

@section project_goals Project Goals and Vision

- @b Mini-Game Variety: Create diverse, fun games that stand alone but integrate well (e.g., puzzles, arcade, memory-based).
- @b Unified Experience: Use the lobby as a playful menu system - no traditional UI lists; instead, physical navigation adds engagement.
- @b Technical Focus: Stick to C for performance and learning; leverage raylib for cross-platform simplicity without heavy dependencies.
- @b Extensibility: Design with future features in mind, like local saves, leaderboards, or basic multiplayer.
- @b Collaboration-Friendly: Monorepo with clear workflows, style guides, and templates to onboard contributors quickly.

Potential expansions (see @ref todo for priorities):
- More games: Snake, Block-Blast, Solitaire, etc.
- Persistent data: High-scores, unlocked skins, settings.
- Polish: Audio configs, fullscreen toggles, error recovery.

@section quick_start Getting Started: Build and Run

Start from the repository root. The Makefile supports incremental builds, multiple modes (release/debug/sanitizers), and targets for libs, executables, and tests.

Common commands:

[code block]bash
# Build libraries (if needed) + lobby executable (incremental)
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
[code block]

Modes include release (optimized), debug (symbols + warnings), strict-debug (warnings as errors), clang-debug (sanitizers), and valgrind-debug (memory checks at runtime).  
For asset paths, use -DASSET_PATH="custom/path/" if needed.

Detailed root Makefile guide: @ref root_makefile "Root Makefile documentation"

Inside a sub-project folder (e.g., tetris/ or a new game):

[code block]bash
# Standalone build + run
make rebuild run-main

# Tests
make run-tests MODE=clang-debug

# Clean
make clean
[code block]

Sub-project Makefile details: @ref sub_makefile "Sub-Project Makefile documentation"

@section repo_structure Repository Structure

The monorepo uses a flat layout with shared folders at the top and per-game subfolders.

[code block]text
.
├── assets/                     # textures, sounds, fonts (shared + game-specific subfolders)
├── docs/                       # all project documentation (this site source)
│   ├── API_Conversion.md       # guide: standalone game -> lobby-compatible API
│   ├── CodeStyleAndConventions.md # style rules
│   ├── makefile.md             # root-level build commands & modes
│   └── ...                     # more shared guides
├── firstparty/                 # utilities & types used by lobby and every game
│   ├── APIs/                   # copied game API headers (e.g., tetrisAPI.h)
│   ├── logger.h                # logging system
│   ├── baseTypes.h             # types (u8, f32Vector2, etc.)
│   ├── configs.h               # constants (paths, physics)
│   ├── utils.h                 # helpers (clamp, randint, etc.)
│   ├── generalAPI.h            # base game struct + errors
│   └── ...                     # more utilities
├── thirdparty/                 # external libs (nob.h for dynamic arrays, etc.)
├── lobby/                      # hub code
│   ├── src/                    # main.c, lobbyAPI.c, etc.
│   ├── include/                # headers
│   ├── tests/                  # unit tests
│   ├── Makefile                # sub-build
│   └── makefile.md             # sub-docs
├── tetris/                     # example integrated game
│   ├── src/                    # tetrisAPI.c, core/board.c, etc.
│   ├── include/                # headers
│   ├── tests/                  # tests
│   ├── Makefile                # sub-build
│   └── makefile.md             # sub-docs
├── sub-project-example/        # template for new games
├── Makefile                    # root build
├── LICENSE.md                  # license
├── CONTRIBUTING.md             # contribution guide
├── CHANGELOG.md                # changes
└── TODO.md                     # tasks
[code block]

After merging more games, they appear as top-level folders like tetris/.

@section core_modules Key Modules and Components

@subsection lobby_module Lobby Module
Serves as the main entry point and scene manager.

- @b Physics and Movement: Basic platformer with gravity, jumps, coyote time, friction (tunable in configs.h).
- @b UI Elements: Skin toggle button, texture menu, debug hitboxes.
- @b Game Switching: Detects player collision with game hitboxes, loads via API (e.g., tetris_initGame), runs loop until running=false.
- @b Configs: Handles video/audio settings via systemSettings.h.

Core files: lobby/src/main.c (entry), lobbyAPI.h/c (API), userTypes.h (structs/enums like Player_st, GameScene_Et).

See: @ref lobby "Lobby Module Overview"

@subsection tetris_module Tetris Module
A faithful classic Tetris clone.

- @b Gameplay: Random tetrominoes, rotation/move/drop, collision, line clears.
- @b Progression: Score based on clears (multiplied by level), speed ramps up.
- @b UI: Board render, next piece preview, score/level display.
- @b Integration: Opaque TetrisGame_St, init/loop/free functions.

Core files: tetrisAPI.h/c (API), core/board.h (logic), ui/board.h (render).

See: @ref tetris "Tetris Module Overview"

@subsection shared_utils Shared Utilities
Reusable across all parts.

- @b Logging: logger.h - levels (info/warn/error), colors, file output in debug, stack traces on Linux.
- @b Types: baseTypes.h - fixed-width (u8/s32), vectors (u32Vector2), shorthands (uint/iVector2).
- @b Configs: configs.h - ASSET_PATH, WINDOW_WIDTH, GRAVITY, JUMP_FORCE, etc.
- @b Utils: utils.h - macros (clamp, swap, UNREACHABLE), functions (randint, getTextureRec).
- @b API Base: generalAPI.h - Error_Et (OK, ERROR_ALLOC, etc.), BaseGame_St (running, score, freeGame).
- @b System Settings: systemSettings.h - VideoSettings_St/AudioSettings_St, apply functions.

Globals like windowRect, appFont in globals.h.

@section dev_workflow Development Workflow

- @b New Game: Copy sub-project-example/, work on branch, merge when ready.
- @b API Conversion: Make game lobby-compatible - opaque struct, embed BaseGame_St, provide init/loop/free.
- @b Commits: Use prefixes (feat:, fix:, docs:, refactor:, etc.) - see @ref contributing.
- @b Reviews: Run tests/sanitizers before PR; ping group for feedback.
- @b Docs: Update @ref changelog for big changes; use Doxygen comments in code.

Full guide: @ref contributing "CONTRIBUTING.md"

Style rules: @ref code_style "CodeStyleAndConventions.md"

Integration steps: @ref api_conversion "API_Conversion.md"

@section testing_debug Testing and Debugging

- @b Modes: Use clang-debug for sanitizers, valgrind-debug for leaks.
- @b Tests: Per-module (tests/ folders), run via make run-tests - logs in timestamped folders.
- @b Logging: VERBOSE=1 for build commands; log_* macros for runtime.
- @b Tools: make run-gdb for debugging main.

@section roadmap Roadmap and Open Tasks

Short-term: Integrate more games, add saves/leaderboards.  
Long-term: Multiplayer modes, online features?  

Details: @ref todo "TODO.md"

@section license License

zlib/libpng - permissive for use/distribution.  

Full: @ref license "LICENSE.md"

Thanks to raylib (Ramon Santamaria), nob.h (Tsoding), and others - credits in headers.

If something is unclear, check the group chat or open an issue/PR.

Happy hacking!

**Created:** February 2026  
**Last updated:** March 02, 2026  
**Author:** [Fshimi Hawlk](https://github.com/Fshimi-Hawlk)