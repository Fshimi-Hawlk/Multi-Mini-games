# Snake Mini-Game

## Overview
Snake is a classic arcade game where the player controls a growing snake that moves around a grid. The goal is to eat apples to grow and increase your score while avoiding colliding with walls or the snake's own body.

## Features
- **Classic Gameplay**: Traditional snake movement and growth mechanics.
- **Score Tracking**: High score preservation in `assets/highScore.txt`.
- **Procedural Apple Spawning**: Apples appear randomly on empty tiles.
- **Dynamic Difficulty**: As the snake grows, the challenge increases.

## Controls
- **W / Up Arrow**: Move Up
- **S / Down Arrow**: Move Down
- **A / Left Arrow**: Move Left
- **D / Right Arrow**: Move Right
- **R**: Restart game (when available)

## Technical Details
- **Grid-based Movement**: The game board is a 2D grid of tiles.
- **Linked List Snake**: The snake's body is managed using a dynamic linked list for efficient growth and movement.
- **Opaque API**: Integrated into the main lobby via a clean API defined in `snakeAPI.h`.

## Credits
**Last updated: April 14, 2026**  
**Author: Léandre BAUDET**
