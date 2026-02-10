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
2. Each mini-game is developed on its **own branch** (example: `block-blast`, `snake`, ...).
3. When a game is ready (plays well, looks finished, tests pass), we merge that branch into `main`.
4. After merging, the game folder becomes part of `main` and we start connecting it to the lobby.

- Example sub-project template: See `sub-project-example/` (includes structure, Makefile, Doxyfile.min, makefile.md documentation, and README template).

## Repository Structure (Root Level)

```
.
├── assets/                   # Images, sounds, fonts — shared or per game
│   ├── fonts/                # Different fonts used by the games
│   └── sub-project/          # Example per-game subdir (added post-merge) (WIP: no sure if it'll stay olike this)
├── firstparty/               # Reusable single-header libs made by the team
├── thirdparty/               # External deps (raylib static lib, PCG rand, etc.)
│   └── NOTICE                # Third-party license notices & attributions (e.g., for rand.h PCG)
├── sub-project-example/      # Template structure when starting a new game
│   ├── docs/                 # generated documentation
│   ├── include/              # .h files
│   │   ├── core/             # Core logic of the game
│   │   ├── ui/               # Draw/Render of the game/app UI
│   │   ├── setups/           # Init and free of the diverse game/app elements
│   │   └── utils/            # Utils like common.h, globals.h, configs.h, ...
│   ├── src/                  # .c files -> same structure as include
│   ├── tests/                # unit tests
│   ├── CHANGELOG.md          # Per-sub-project detailed changelog
│   ├── Makefile              # Makefile to build the sub-project as a separated unit
│   ├── make/                 # Split of the previously monolith Makefile into digestable parts
│   ├── makefile.md           # Explains make commands to build the sub-project
│   ├── Doxyfile.min          # Minimal Doxygen configs for per-sub-project docs
│   └── README.md             # Template README for new sub-projects
├── docs/                     # (Future) Aggregated/shared documentation across all games/lobby
│   ├── API_Conversion.md     # Explains how to convert a game into an API to be intergrated into the lobby
│   └── makefile.md           # Explains make commands to build the entire project
├── LICENSE                   # Project license
├── CHANGELOG.md              # High-level changes (branch creations, merges, shared updates)
├── CONTRIBUTING.md           # Internal guidelines
├── TODO.md                   # Internal reminders (lobby integration, docs, etc.)
├── .gitignore                # Ignores build/, logs/, docs/, etc.
├── Makefile                  # Build all merged games/lobby at once
└── README.md                 # <-- This file
```

Post-merge example (after a sub-project merge):

```
├── sub-project/              # Merged game: src/, include/, tests/, etc.
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

See `makefile.md` for full documentation.

## Building & Running (Sub-Project Level)

From inside a branch root (e.g., after `git checkout sub-project` or in `sub-project-example/`):

See all options: `make help`
- Default build (release): `make` or `make all`
- Debug/sanitizers: `make MODE=clang-debug` (requires Clang installed)
- Valgrind checks: `make MODE=valgrind-debug run-main` or `make MODE=valgrind-debug run-tests` (requires Valgrind installed)
- Tests: `make run-tests` (live output + logs in `logs/tests-<timestamp>/`)
- Full rebuild + run: `make rebuild run-main`

**Note**: Global root Makefile coming later (for building all merged games/lobby at once — see [TODO](TODO.md)).

## Generating Documentation (Per Game)

Inside a game folder:

```bash
doxygen Doxyfile.min
```

->You can see the generated documentation in: [./docs/html/index.html](./docs/html/index.html).  

**Future**: Root-level docs aggregation (all games + lobby in one view)  