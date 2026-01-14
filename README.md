# Multi Mini-Games

## Cadre :

**Projet de L2 Informatique 2025-2026**
janvier 2026 --> avril 2026

## Membres :

[BAUDET Léandre](Leandre.Baudet.Etu@univ-lemans.fr)
[BERGE Kimi](Kimi.Berge.Etu@univ-lemans.fr)
[CAILLON Charles](Charles_Caillon.Etu@univ-lemans.fr)
[CHAUVEAU Maxime](Maxime.Chauveau.Etu@univ-lemans.fr)

**Collection of independent mini-games, eventually integrated into a unified interactive lobby (single-player with potential multiplayer extensions).**

The lobby will be a platformer-style hub where players navigate to access each mini-game as a "scene" (seamless switch, same window/executable).

Tech stack: Pure C with raylib for graphics/input/audio.

## Project Workflow (Monorepo with Branches)

- Development happens in **dedicated branches** per mini-game (e.g., `block-blast`).
- Shared resources (code, assets, dependencies) live at root and are accessible from branches.
- When a mini-game is stable/complete:
  - Merge branch into `main`.
  - Game code becomes a folder at root (e.g., `block-blast/`).
  - Integrate as lib/module for lobby scene switching.
- Root `main` branch always contains: lobby code (future), shared folders, and merged games.

## Current Active Branch

`block-blast` — Recreation + extensions of *Block Blast!* by Hungry Studio.

For details, switch to the branch and read its `README.md`.

## Repository Structure (Root Level)

```
.
├── assets/                   # Shared + per-game assets (images, fonts, sounds, data for saves/leaderboards)
│   ├── fonts                 # Different fonts used by the games
│   └── block-blast/          # Example per-game subdir (added post-merge)
├── build
│   ├── bin/
│   └── obj/
├── firstparty/               # Reusable single-header libs (stb-style, your own)
├── thirdparty/               # External deps (raylib static lib, PCG rand, etc.)
├── docs/                     # (Future) Aggregated/shared documentation across all games/lobby
├── LICENSE                   # Project license
├── CHANGELOG.md              # High-level changes (branch creations, merges, shared updates)
├── CONTRIBUTING.md           # Guidelines for contributions/PRs
├── TODO.md                   # Internal reminders (global Makefile, lobby integration, etc.)
├── .gitignore                # Ignores build/, logs/, docs/, etc.
├── Makefile                  # (Future global) Build all merged games/lobby at once
└── README.md                 # This file
```

Post-merge example (after block-blast merge):

```
├── block-blast/              # Merged game: src/, include/, tests/, etc.s
```

## Getting Started (Git Commands)

```bash
# Clone the repo
git clone https://github.com/yourusername/multi-mini-games.git
cd multi-mini-games

# List all branches
git branch -a

# Switch to a mini-game branch
git checkout block-blast

# Create/start a new mini-game branch
git checkout -b new-game-name
```

## Building & Running (Sub-Project Level)
From inside a branch root (e.g., after `git checkout block-blast`):

See all options: `make help`
- Default build (release): `make` or `make all`
- Debug/sanitizers: `make MODE=clang-debug` (requires Clang installed)
- Valgrind checks: `make MODE=valgrind-debug run-main` or `make MODE=valgrind-debug run-tests` (requires Valgrind installed)
- Tests: `make run-tests` (live output + logs in logs/tests-<timestamp>/)
- Full rebuild + run: `make rebuild run-main`

**Note**: Global root Makefile coming later (for building all merged games/lobby at once — see [TODO](TODO.md)).

## Generating Documentation (Sub-Project Level)

From branch root:

```bash
doxygen Doxyfile.min
```

Output: [web page](./docs/html/index.html) (open in browser).  
Includes code API + Makefile documentation.  
Generated files are ignored in git — regenerate as needed.  
**Future**: Root-level docs aggregation (all games + lobby in one view)  