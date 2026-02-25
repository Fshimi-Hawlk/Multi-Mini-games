# Multi Mini-Games

## Project Overview

**Second-year Computer Science Bachelor's project (2025-2026)**  
Development period: January 2026 – April 2026

### Members:

- BAUDET Léandre:  Leandre.Baudet.Etu@univ-lemans.fr  
- BERGE Kimi:      Kimi.Berge.Etu@univ-lemans.fr  
- CAILLON Charles: Charles_Caillon.Etu@univ-lemans.fr  
- CHAUVEAU Maxime: Maxime.Chauveau.Etu@univ-lemans.fr  

### Goal

Create several small independent mini-games in C using **raylib**.  
Later, connect them inside one program through a common **lobby** (a simple platformer-style menu world).  
Players walk to a door/zone -> the corresponding mini-game starts in the same window (no new executable).

- Single-player mode (for now)
- Multiplayer mode (planned later)

## Project Workflow

We use a [**monorepo**](https://wellarchitected.github.com/library/scenarios/monorepos/) (one single repository for everything) with **one branch per mini-game**.

1. The stable, shared code lives on branch **`main`**.
2. Each mini-game is developed on its **own branch** (example: `suika`, `block-blast`, `snake`, ...).
3. When a game is ready (plays well, looks finished, tests pass), we merge that branch into `main`.
4. After merging, the game folder becomes part of `main` and we start connecting it to the lobby.

## Root-Level Repo Structure

```
.
├── assets/                     # Images, sounds, fonts - shared or per game
│   └── fonts/                  # Different fonts used by the games
├── docs/                       # Aggregated/shared documentation across all games/lobby
│   ├── API_Conversion.md       # Explains how to convert a game into an API to be integrated into the lobby
│   ├── mainpage.dox            # Doxygen main page
│   └── makefile.md             # Explains make commands to build the entire project
├── firstparty/                 # Reusable single-header libs made by the team
│   ├── APIs/                   # Game APIs for lobby integration
│   ├── logger.h                # Logging system
│   └── baseTypes.h             # Base type definitions
├── thirdparty/                 # External libraries and headers
├── lobby/                      # The lobby follows the same structure as games
├── suika/                      # Suika game (Watermelon Game)
├── LICENSE                     # Project license
├── CHANGELOG.md                # High-level changes (branch creations, merges, shared updates)
├── CONTRIBUTING.md             # Internal guidelines
├── TODO.md                     # Internal reminders (lobby integration, docs, etc.)
├── CodeStyleAndConventions.md  # Code style guidelines
├── .gitignore                  # Ignores build/, logs/, docs/, etc.
└── README.md                   # <-- This file
```

## Sub-Project-Level Repo structure

```
sub-project/
├── assets/                 # Sub-project-specific images, sounds, etc.
├── docs/                   # generated documentation
├── include/                # .h files
│   ├── core/               # Core logic of the game
│   ├── ui/                 # Draw/Render of the game/app UI
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

## Building & Running (Sub-Project Level)

From inside a game folder (e.g., `suika/` or `lobby/`):

See all options: `make help`
- Default build (release): `make` or `make all`
- Debug/sanitizers: `make MODE=clang-debug` (requires Clang installed)
- Valgrind checks: `make MODE=valgrind-debug run-main` or `make MODE=valgrind-debug run-tests` (requires Valgrind installed)
- Tests: `make run-tests` (live output + logs in `logs/tests-<timestamp>/`)
- Full rebuild + run: `make rebuild run-main`

## Generating Documentation

### At Project-Root-level

```bash
cd lobby && make docs
```

-> open [docs/doxygen/html/index.html](./docs/doxygen/html/index.html)

### Per Game

Inside a game folder:

```bash
make docs
```

-> You can see the generated documentation in: `docs/html/index.html`.

## Credits

**Last updated: February 24, 2026**  
**Author: [Fshimi Hawlk](https://github.com/Fshimi-Hawlk)**
