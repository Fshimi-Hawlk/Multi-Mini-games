# TODO.md — Multi Mini-Games Monorepo

High-level reminders for root/main branch tasks.  
Prioritized roughly by importance/dependency order.

## Immediate / Setup

- [ ] Create root-level global Makefile  
  Wrapper to build everything at once (lobby + all merged sub-games as libs).  
  Possible targets: `make all`, `make lobby`, `make block-blast` (post-merge), clean-all, etc.

- [ ] Add root `docs/` folder  
  Purpose: Aggregated/shared documentation.  
  - Top-level Doxyfile for documenting lobby code + firstparty/shared utils.  
  - Optional: Index page linking to per-game docs (`block-blast/docs/html/`, etc.).  
  - Later: Automated generation of combined API reference across all merged games.

## Integration (Post First Merge)

- [ ] Define sub-game integration standard  
  - Each merged game compiled as static lib.  
  - Public header per game (e.g., `blockBlastAPI.h` with prefixed functions, e.g., `blockBlast_init()`, `blockBlast_update()`, `blockBlast_draw()`, `blockBlast_cleanup()`, ...).  
  - Lobby calls these for seamless scene switching (reuse same window, unload/load assets).

- [ ] Shared save/load framework  
  - Common utils in firstparty/ for persisting state (JSON or binary).  
  - Per-game data in `assets/<game>/saves/` or `assets/<game>/leaderboards/`.

- [ ] Leaderboards / persistent data structure  
  - Decide format (local files, simple DB, or in-memory for now).  
  - Subdirs like `assets/block-blast/data/`.

## Future

- [ ] Roadmap section in root README (checklist of merged games, lobby progress).