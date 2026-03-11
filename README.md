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
Players walk to a door/zone → the corresponding mini-game starts in the same window (no new executable).

- Single-player mode (for now)
- Multiplayer mode (planned later)

## Project Workflow

We use a [**monorepo**](https://wellarchitected.github.com/library/scenarios/monorepos/) (one single repository for everything) with **one branch per mini-game**.

1. The stable, shared code lives on branch **`main`**.
2. Each mini-game is developed on its **own branch** (example: `solitaire`, `tetris`, ...).
3. When a game is ready (plays well, looks finished, tests pass), we merge that branch into `main`.
4. After merging, the game folder becomes part of `main` and we start connecting it to the lobby.

## Current Games

- **Solitaire (Klondike)** by Maxime CHAUVEAU - Classic card game with drag-and-drop

## Repository Structure (Root Level)

```
.
├── assets/                   # Images, sounds, fonts — shared or per game
│   ├── fonts/                # Different fonts used by the games
│   └── solitaire/           # Solitaire card textures
├── docs/                     # Documentation (bilingual: en/fr)
│   └── doxygen/            # Doxygen documentation
├── firstparty/               # Reusable single-header libs made by the team
├── thirdparty/               # External deps (raylib static lib, PCG rand, etc.)
│   └── NOTICE              # Third-party license notices
├── lobby/                   # Main lobby (platformer-style menu)
├── solitaire/               # Solitaire (Klondike) card game
├── LICENSE                  # Project license
├── CHANGELOG.md             # High-level changes
├── CONTRIBUTING.md          # Guidelines for contributors
├── TODO.md                  # Internal reminders
├── .gitignore              # Ignores build/, logs/, docs/, etc.
├── Makefile                # Root-level build system
└── README.md               # <-- This file
```

## Building & Running

### Root Level

```bash
make help               # see all targets
make bin                # build libraries + lobby executable
make rebuild-exe        # force rebuild lobby executable only
make run-exe            # run the lobby
make run-tests          # run all tests across modules
```

### Per Game (e.g., Solitaire)

```bash
cd solitaire
make help               # see all targets
make                    # build executable
make run-main           # build and run
```

### Build Modes

- **Release**: `make MODE=release` (default)
- **Debug**: `make MODE=debug`
- **Strict Debug**: `make MODE=strict-debug`
- **Clang Debug**: `make MODE=clang-debug` (requires Clang)
- **Valgrind Debug**: `make MODE=valgrind-debug` (requires Valgrind)

## Documentation

### Project Documentation

- **English**: [docs/doxygen/en/](./docs/doxygen/en/)
- **Français**: [docs/doxygen/fr/](./docs/doxygen/fr/)

### Game Documentation

Each game has its own documentation:
- [solitaire/CHANGELOG.md](./solitaire/CHANGELOG.md)
- [solitaire/CORRECTIONS.md](./solitaire/CORRECTIONS.md)

### API Documentation

See [docs/doxygen/](./docs/doxygen/) for full API documentation.

## Code Style

See [CodeStyleAndConventions.md](./CodeStyleAndConventions.md) for our coding conventions.

## Credits

**Created: January 2025**  
**Last updated: March 2026**  
**Authors: Fshimi Hawlk, Maxime CHAUVEAU, Léandre BAUDET, Kimi BERGE, Charles CAILLON**
