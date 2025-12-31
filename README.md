# Multi-Mini-games
Collection of multiple sub-projects/mini-games playable through a single-player/multi-player platformer-style interactive lobby

The project is devided in branch per sub-project. 


## Current branch: Block Blast
The initial Block Blast game, by Hungry Studio, is an endless-mode block puzzle on an 8x8 grid where you place three polyomino-style shapes per turn to clear full rows, columns, or both for points. Game over occurs when none of the remaining shapes can be placed anywhere on the board.

The idea for this sub-project is to first reproduce the game, then add new features. Reproducing here means have a board, 3 "prefabs" polyomino-style shapes that can be placed on the board, and the different algorithms to:
 - generate the given prefabs 
 - compute the score when 
   - placing 
   - clearing rows/colums
   - cleared streaks

The feature to be added afterward are the following:
 - New prefabs:
    The base game have the usual polyomino-style shapes, e.g. unit, T, L, z, +, | 2/3/4/5-wide, 2x3, 3x3 (Note: there are also mirror variants for some of them), which is already a good amount of prefabs but more will certainly make the game more interesting and complex.
 - Different board layout:
    The base game include only one board shape of 8x8 tiles. So i'm adding more, not just only different size square, but a whole slew of shapes: rectangle, circle, other polygons, ... And some layout with holes in them to make them more varied.
 - Tile but with durability:
    Some tile may require more than one clear to destroy them, tougher tile if you want. They could be recognize via some visual changes.
 - Save/Load:
    As a game can last some time, the player can decided to save the current state of the game, and then quit. So then later they can load and continue where they left.