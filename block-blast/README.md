# Documentation

## Brief

A faithful **reproduction** + **feature expansion** of the popular mobile puzzle *Block Blast!* by [**Hungry Studio**](https://www.hungrystudio.com/).

## Overview

*Block Blast!* (released ~2022–2023) is a relaxing, endless block-placement puzzle played on an **8×8 grid**.

### Core Gameplay (Original Game)
- Each turn, **three random polyomino shapes** appear at the bottom.
- Drag and drop them onto the grid (no rotation in classic mode).
- Fill any **full row** or **full column** → it clears automatically, awarding points.
- **Combos & streaks** give bonus points (e.g., clearing multiple lines at once or consecutively).
- Game over when none of the three shapes can be placed anywhere on the board.

The original is known for its calm pace, no timers, offline play, and addictive "just one more turn" feel. It has millions of downloads and remains a top free puzzle game on iOS/Android.

→ Official app links:  
[App Store (iOS)](https://apps.apple.com/us/app/block-blast/id1617391485) · [Google Play (Android)](https://play.google.com/store/apps/details?id=com.block.juggle)

## Project Goals

This sub-project aims to:

1. **Faithfully recreate the classic mode**  
   Including:
   - 8×8 square grid
   - Generation of random polyomino shapes (the usual set: monomino, domino, trominoes, tetrominoes like T/L/Z/S/I/O, plus some pentominoes, cross/plus, 2×3 rectangles, etc. — including mirror variants)
   - Placement logic
   - Row/column clearing
   - Basic scoring (with combo/streak multipliers)

2. **Extend the game with new features** (post-reproduction phase)

### Planned New Features
- **Expanded shape set**  
  More polyominoes and custom shapes beyond the originals to increase variety and strategic depth.

- **Custom board layouts**  
  Beyond the classic 8×8 square:  
  - Different sizes (e.g. 7×10 rectangles)  
  - Non-rectangular shapes (circles, hexagons, polygons…)  
  - Boards with **holes/obstacles** for added challenge

- **Tiles with durability**  
  Some cells require multiple clears to destroy (e.g. "tough" or "armored" tiles) — visually distinct.

- **Save / Load system**  
  Persist game state so players can quit and resume long sessions.

## Tech / Structure Notes
- Language: C
- Folders: `src/`, `include/`, `tests/`, `assets/`, etc.
- Documentation: Generated via Doxygen → see [`docs/html/index.html`](./docs/html/index.html) (after running `doxygen`)

Contributions, ideas, or bug reports are welcome!
