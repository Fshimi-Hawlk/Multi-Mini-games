@page readme Project README

# Multi Mini-Games

@section overview Project Overview

[REDACTED]

@subsection goal Goal

Create several small independent mini-games in C using @b raylib.  
Later, connect them inside one program through a common @b lobby (a simple platformer-style menu world).  
Players walk to a door/zone -> the corresponding mini-game starts in the same window (no new executable).

- Single-player mode (for now)
- Multiplayer mode (planned later)

@section workflow Project Workflow

We use a @b monorepo (one single repository for everything) with @b one branch per mini-game.

1. The stable, shared code lives on branch @b main.
2. Each mini-game is developed on its @b own branch (example: @c block-blast, @c snake-classic, @c memory-game).
3. When a game is ready (plays well, looks finished, tests pass), we merge that branch into @c main.
4. After merging, the game folder becomes part of @c main and we start connecting it to the lobby.

- Example sub-project template: See @c sub-project-example/ (includes structure, Makefile, Doxyfile.min, makefile.md documentation, and README template).

@section repo_structure Root-Level Repo Structure

```text
.
├── assets/                     # Images, sounds, fonts - shared or per game
│   └── fonts/                  # Different fonts used by the games
├── docs/                       # Aggregated/shared documentation across all games/lobby
│   ├── API_Conversion.md       # Explains how to convert a game into an API to be intergrated into the lobby
│   └── ...                     # more
├── firstparty/                 # Shared C code
├── thirdparty/                 # External libs
├── lobby/                      # Hub code
├── tetris/                     # Integrated game
├── sub-project-example/        # Template
├── Makefile                    # Root build
├── LICENSE.md
├── CONTRIBUTING.md
├── CHANGELOG.md
└── TODO.md
```

@section building_root Building & Running (Root Level – after games are merged)

From the repository root:

```bash
make help               # see all targets
make bin                # build libraries (if needed) + lobby executable
make rebuild-exe        # force rebuild lobby executable only
make run-exe            # run the lobby
make run-tests          # run all tests across modules
```

See @ref root_makefile for full documentation.

@section building_sub Building & Running (Sub-Project Level)

From inside a branch root (e.g., after @c git checkout sub-project or in @c sub-project-example/):

See all options: @c make help
- Default build (release): @c make or @c make all
- Debug/sanitizers: @c make MODE=clang-debug (requires Clang installed)
- Valgrind checks: @c make MODE=valgrind-debug run-main or @c make MODE=valgrind-debug run-tests (requires Valgrind installed)
- Tests: @c make run-tests (live output + logs in @c logs/tests-<timestamp>/)
- Full rebuild + run: @c make rebuild run-main

@note Global root Makefile coming later (for building all merged games/lobby at once - see @ref todo).

@section gen_docs Generating Documentation

@subsection root_level At Project-Root-level

```bash
make docs
```

-> open [docs/index.html](./docs/index.html)

@subsection per_game Per Game

Inside a game folder:

```bash
make docs
```

-> You can see the generated documentation in: [./docs/html/index.html](../../docs/html/index.html).

**Created:** March 02, 2026  
**Author:** [Fshimi Hawlk](https://github.com/Fshimi-Hawlk)