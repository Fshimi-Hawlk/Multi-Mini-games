# Multi Mini-Games

**A collection of mini-games playable through a unified interactive lobby (single-player or multiplayer platformer-style hub).**

This is a monorepo-style project where each mini-game lives in its own **dedicated branch**. The main branch (`main`) contains:
- Shared code & assets (`firstparty/`, `thirdparty/`, `assets/`)
- The lobby/hub code
- Common utilities (build system, save/load framework, etc.)

## Project Structure

```
.
├── main/                     # Lobby & hub code (platformer-style menu world)
├── firstparty/               # Your reusable single-header libs (stb-style)
├── thirdparty/               # External dependencies
├── assets/                   # Shared fonts, images, sounds, levels, etc.
├── docs/                     # (optional) Aggregated or shared documentation
├── README.md                 # This file
├── CHANGELOG.md              # (recommended) Overall project changes
├── LICENSE                   # (add one!)
└── .gitignore
```


Each mini-game is developed in its own branch, e.g.:
- `block-blast` → current active branch
- `future-game-1`, `future-game-2`, etc. (once started)

When a game is "done" (or reaches a milestone), it can be merged into `main`.

## Current Active Branch: block-blast

This branch implements a recreation + extension of **Block Blast!** by [Hungry Studio](https://www.hungrystudio.com/) — a hugely popular relaxing block puzzle (70M+ DAU reported in 2026).

### Original Game Rules (Confirmed)
- Endless puzzle on an **8×8 grid**.
- Each turn: **three random polyomino shapes** appear (no rotation allowed).
- Drag & drop to place them.
- Fill any complete row **or** column → clears automatically, scores points.
- **Combos/streaks** (multiple clears in one placement or consecutively) give bonus points.
- Game over when no shape can be placed anywhere.
- Relaxing, no timer, offline-friendly.

→ Play the original: [iOS App Store](https://apps.apple.com/us/app/block-blast/id1617391485) | [Google Play](https://play.google.com/store/apps/details?id=com.block.juggle)

### This Sub-Project Goals
1. **Faithful classic mode recreation**  
   - 8×8 grid  
   - Random shape generation (classic polyominoes: monomino, domino, T/L/Z/S/I/O, plus, lines 2–5, 2×3, 3×3, mirrors/variants)  
   - Placement, row/column clearing, basic scoring + combo/streak logic

2. **Planned extensions** (after core is solid)  
   - Expanded shape pool  
   - Custom board layouts (rectangles, circles, polygons, boards with holes)  
   - Tiles with durability (require multiple clears; visually distinct)  
   - Save/load system for long sessions

→ See detailed README in the `block-blast` branch:  
[`block-blast/README.md`](./block-blast/README.md) (or switch to branch to view it)

## How to Work with Branches

- List branches: `git branch -a`
- Switch to a game: `git checkout block-blast`
- Create new game branch: `git checkout -b new-game-name`
- Build/run: (add instructions here once you have a Makefile/CMake setup)  
  e.g. `make` from root, or `cd block-blast && make`

## Roadmap / Status

- [x] Lobby skeleton (basic platformer hub)
- [ ] First playable game: Block Blast (in progress on `block-blast` branch)
- [ ] Shared save/load framework
- [ ] Second game slot
- [ ] Multiplayer lobby features (TBD)