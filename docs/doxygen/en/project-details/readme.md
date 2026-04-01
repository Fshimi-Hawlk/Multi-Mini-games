@page readme README

@ref index "Back to Home"

**Last checked against project structure:** March 16, 2026

# Multi Mini-Games

## Project Overview

**Second-year Computer Science Bachelor's project (2025-2026)**  
Development period: January 2026 – April 2026

### Members:

- BAUDET Léandre:  Leandre.Baudet.Etu@univ-lemans.fr  
- BERGE Kimi:      Kimi.Berge.Etu@univ-lemans.fr  
- i-Charlys: Charles_Caillon.Etu@univ-lemans.fr  
- CHAUVEAU Maxime: Maxime.Chauveau.Etu@univ-lemans.fr  

### Goal

Create several small independent mini-games in C using **raylib**.  
Later on, connect them inside one single program through a common **lobby** — a simple platformer-style hub world.  
Players walk around, reach a door or collision zone -> the chosen mini-game starts right there in the same window (no spawning new executables, no extra processes).

- Single-player focus for now
- Multiplayer mode planned for later (local co-op or basic online)

## Project Workflow

We are using a **monorepo** setup (everything lives in one repository) with **one branch per mini-game**.

1. Stable shared code + merged games live on branch **main**.
2. Each new mini-game gets developed on its **own dedicated branch** (examples: `block-blast`, `snake-classic`, `tetris`, `memory-game`, etc.).
3. When a game is in good shape (gameplay solid, visuals decent, tests passing, API ready), we merge that branch into `main`.
4. Once merged, the game folder becomes permanent on `main` and we work on hooking it up to the lobby.

- New game starter template: look at folder `sub-project-example/`  
  It already contains the recommended folder layout, Makefile, minimal Doxyfile, `makefile.md`, and a README stub.

## Root-Level Repository Structure

```text
.
├── assets/                     # shared + per-game images, sounds, fonts
│   └── fonts/                  # fonts used across games
├── docs/                       # shared documentation + Doxygen output
│   ├── API_Conversion.md       # how to turn a standalone game into lobby-compatible API
│   ├── makefile.md             # root make commands explained
│   └── ...                     # other guides
├── firstparty/                 # our own reusable headers & single-file libs
├── thirdparty/                 # external dependencies + NOTICE file for licenses
├── lobby/                      # the hub/world code
├── tetris/                     # example of already integrated game
├── sub-project-example/        # copy-paste template for starting new games
├── Makefile                    # root-level build system (libs + lobby exe)
├── LICENSE.md
├── CONTRIBUTING.md
├── CHANGELOG.md
├── TODO.md
└── .gitignore
```

After more games get merged you will see additional top-level folders like `block-blast/`, `snake-classic/`, etc.

## Sub-Project-Level Structure (each game / lobby)

```text
sub-project/
├── assets/                 # game-specific images, sounds, fonts, etc.
├── docs/                   # per-game generated html (after make docs)
├── include/                # all headers
│   ├── core/               # game logic headers
│   ├── ui/                 # rendering / drawing code headers
│   ├── setups/             # init & cleanup functions
│   ├── utils/              # helpers, globals, configs, common types
│   └── subProjectAPI.h     # lobby-facing API header
├── src/                    # implementation files (mirrors include/ + main.c)
├── tests/                  # unit tests
├── CHANGELOG.md            # detailed per-game changes
├── Makefile                # build this sub-project standalone or as lib
├── make/                   # split Makefile logic into smaller files
├── makefile.md             # commands & modes explained for this sub-project
├── Doxyfile.min            # minimal Doxygen config for local docs
└── README.md               # per-game readme (template provided)
```

## Building & Running (Root Level – merged games + lobby)

From the repository root:

```bash
make help               # quick list of all useful targets
make bin                # build needed libs + link the lobby executable
make rebuild-exe        # force relink lobby exe (useful after lib changes)
make run-exe            # launch the lobby
make run-tests          # run tests from all modules (lobby + merged games)
```

Full command list and modes (debug, sanitizers, valgrind, etc.) explained in @ref root_makefile.

## Building & Running (Inside a single sub-project / branch)

From inside `sub-project-example/`, `lobby/`, `tetris/`, or any game folder:

```bash
make help               # see available targets
make                    # default build (release mode)
make MODE=clang-debug   # debug + address/undefined sanitizers (needs clang)
make MODE=valgrind-debug run-main   # run with valgrind memcheck
make run-tests          # execute unit tests + save logs
make rebuild run-main   # clean build then run the standalone main
```

Note: root-level Makefile for building *everything* at once is still WIP — track progress in @ref todo.

## Generating Documentation

### Root-level (whole project)

```bash
make docs
```

-> open `docs/doxygen/index.html`

### Per sub-project / game

Inside any game or lobby folder:

```bash
make docs
```

-> Open `./docs/html/index.html`.

**Created:** December 16, 2025  
**Last updated:** March 16, 2026  
**Author:** [Fshimi Hawlk](https://github.com/Fshimi-Hawlk)

@ref index "Back to Home"