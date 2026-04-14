# Tetromino Fall

## Overview
Tetromino Fall is a polished Tetris clone where players must arrange falling geometric shapes (tetrominoes) into complete horizontal lines. Completed lines disappear, granting points and clearing space for more shapes. The game ends if the shapes stack up to the top of the board.

## Features
- **All 7 Tetrominoes**: Fully implemented I, J, L, O, S, T, and Z shapes with accurate rotation.
- **Line Clearing**: Automatic detection and clearing of multiple lines with scoring bonuses.
- **Ghost Piece**: Visual aid showing where the current piece will land.
- **Persistent High Score**: Tracks your best performance across sessions.
- **Auto-Play Mode**: Built-in algorithm capable of playing the game autonomously.
- **Dynamic Speed**: Gravity increases as you clear more lines, ramping up the difficulty.

## Controls
- **Left / Right Arrow**: Move piece sideways
- **Down Arrow**: Soft drop (accelerate fall)
- **Up Arrow**: Rotate piece clockwise
- **Space**: Hard drop (instantly land piece)
- **P**: Pause game

## Technical Details
- **Rotation System**: Implements standard rotation mechanics with collision-aware wall kicks.
- **Efficient Board Representation**: Uses a compact 2D array for the game state.
- **Integration Ready**: Follows the common mini-game API for seamless inclusion in the lobby.

## Credits
**Last updated: April 14, 2026**  
**Author: Léandre BAUDET**
