@page readme README

## Project Overview

[REDACTED]

### Goal

Create several small independent mini-games in C using **raylib**.  
Later, connect them inside one program through a common **lobby** (a simple platformer-style menu world).  
Players walk to a door/zone -> the corresponding mini-game starts in the same window (no new executable).

- Single-player mode (for now)
- Multiplayer mode (planned later)

## Project Workflow

We use a **monorepo** (one single repository for everything) with **one branch per mini-game**.

1. The stable, shared code lives on branch **main**.
2. Each mini-game is developed on its **own branch** (example: `block-blast`, `snake-classic`, `memory-game`).
3. When a game is ready (plays well, looks finished, tests pass), we merge that branch into `main`.
4. After merging, the game folder becomes part of `main` and we start connecting it to the lobby.

- Example sub-project template: See `sub-project-example/` (includes structure, Makefile, Doxyfile.min, makefile.md documentation, and README template).

## Root-Level Repo Structure

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

## Building & Running (Root Level – after games are merged)

From the repository root:

```bash
make help               # see all targets
make bin                # build libraries (if needed) + lobby executable
make rebuild-exe        # force rebuild lobby executable only
make run-exe            # run the lobby
make run-tests          # run all tests across modules
```

See @ref root_makefile for full documentation.

## Building & Running (Sub-Project Level)

From inside a branch root (e.g., after `git checkout sub-project` or in `sub-project-example/`):

See all options: `make help`
- Default build (release): `make` or `make all`
- Debug/sanitizers: `make MODE=clang-debug` (requires Clang installed)
- Valgrind checks: `make MODE=valgrind-debug run-main` or `make MODE=valgrind-debug run-tests` (requires Valgrind installed)
- Tests: `make run-tests` (live output + logs in `logs/tests-<timestamp>/`)
- Full rebuild + run: `make rebuild run-main`

**Note**: Global root Makefile coming later (for building all merged games/lobby at once - see @ref todo).

## Generating Documentation

### At Project-Root-level

```bash
make docs
```

-> open `docs/index.html`

### Per Game

Inside a game folder:

```bash
make docs
```

-> You can see the generated documentation in: `./docs/html/index.html`.

**Created:** March 02, 2026  
**Author:** [Fshimi Hawlk](https://github.com/Fshimi-Hawlk)