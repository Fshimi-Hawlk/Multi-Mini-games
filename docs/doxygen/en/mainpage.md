@mainpage Home page

# Multi Mini-Games

**Last checked against project structure:** March 22, 2026  

This monorepo hosts a collection of small, independent mini-games built in C with raylib.  
The core idea is one single executable: a central **lobby** (simple 2D platformer hub) where you walk around and trigger games by walking into collision zones. Games run inside the lobby, no extra windows or processes.

As of March 2026 the project has:
- Working lobby with movement, physics, and basic UI (skin selection).
- Four integrated games: Tetris, Solitaire, Suika, Bowling.
- Shared logging, types, utilities, and the API layer for adding more games.
- Build system and documentation setup.

@ref project_details "Project details"  
@ref build_system "Build system"  
@ref team_guidelines "Team guidelines"

## Quick Start

From repository root:

```bash
make libs      # build shared libs
make bin       # link lobby executable
make run-exe   # run the lobby
make tests     # run all tests
```

Full commands and modes (debug, sanitizers, valgrind, sub-project builds) -> @ref root_makefile "Root Makefile documentation"

Inside any game folder (jeux/tetris/, jeux/solitaire/, etc.):

```bash
make rebuild run-main
make run-tests
```

Sub-project details stay in each game’s makefile.md.

## Repository Structure

```text
.
├── assets/
├── docs/                       # this site source
├── firstparty/                 # shared utils, logger, types, generalAPI
├── thirdparty/
├── lobby/
├── jeux/                       # all mini-game sub-projects
│   ├── tetris/
│   ├── solitaire/
│   ├── suika/
│   └── bowling/
├── sub-project-example/        # copy for new games
├── Makefile                    # root
├── LICENSE.md
├── CONTRIBUTING.md
├── CHANGELOG.md
└── TODO.md
```

Games live in `jeux/` and are discovered automatically by the root Makefile.

## Current Modules

### Lobby Module
Main entry point and scene manager.  
Collision detection loads games via API.  
See: @ref lobby "Lobby Module Overview" (add page later)

### Tetris Module
Classic Tetris with score, levels, next-piece preview.  
Integrated through opaque struct + init/loop/free.  
See: @ref tetris "Tetris Module Overview" (add page later)

### Solitaire Module
Klondike Solitaire with card drag-and-drop, win detection, and audio feedback.  
Integrated via `solitaire_initGame` / `solitaire_gameLoop` / `solitaire_freeGame`.

### Suika Module
Watermelon Game (fruit-merge physics).  
Integrated via `suika_initGame` / `suika_gameLoop` / `suika_freeGame`.

### Bowling Module
3D bowling lane with physics simulation, score tracking, and sound effects.  
Integrated via `bowling_initGame` / `bowling_gameLoop` / `bowling_freeGame`.

Shared stuff (logger, baseTypes, configs, utils, generalAPI) lives in firstparty/.

## Development Workflow & Reminders

- New game: copy sub-project-example/, branch, merge when API-ready.
- API conversion steps -> @ref api_conversion
- Commit style -> @ref contributing
- Code style -> @ref code_style_and_conventions
- Tests + sanitizers before any merge.
- Update @ref changelog and @ref todo after big changes.

Full guide: @ref contributing "CONTRIBUTING.md"

## Roadmap and Open Tasks

Short-term: more games, saves, leaderboards.  
Long-term ideas in TODO.md.

Details: @ref todo "TODO.md"

## License

zlib/libpng. Full text: @ref license "LICENSE.md"

Thanks to raylib, nob.h, etc. (credits in headers).

**Created:** March 01, 2026  
**Last updated:** March 16, 2026  
**Author:** [Fshimi Hawlk](https://github.com/Fshimi-Hawlk)