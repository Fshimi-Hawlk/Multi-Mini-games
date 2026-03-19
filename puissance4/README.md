# Puissance4 (Connect Four)

**Two-player strategy game where players compete to connect four discs in a row**

## Overview

Puissance4 (also known as Connect Four) is a classic two-player strategy game where players take turns dropping colored discs into a vertical grid. The first player to connect four discs of their color in a row wins!

### Core Gameplay

- **6x7 grid**: The game is played on a 6-row by 7-column vertical board
- **Two players**: Player 1 (Red) vs Player 2 (Yellow)
- **Alternating turns**: Players take turns dropping one disc per turn
- **Gravity**: Discs fall to the lowest available position in the chosen column
- **Win condition**: First to connect 4 discs horizontally, vertically, or diagonally wins
- **Draw**: If the board is filled without a winner, the game is a draw

### Controls

- **Mouse Click**: Click on a column to drop your piece
- **ESC**: Quit the game
- **SPACE**: Restart the game (when game is over)

## Project Goals

- Provide a fun, playable Connect Four game
- Demonstrate proper C game development practices
- Create modular, well-documented code
- Support integration with the Multi-Mini-Games framework

## Tech Stack

- **Language**: C
- **Graphics Library**: RayLib
- **Documentation**: Doxygen
- **Build System**: Make

## Project Structure

```
puissance4/
├── include/          # Header files
│   ├── puissance4API.h   # Main game API
│   ├── audio.h           # Audio module
│   ├── core/             # Core game definitions
│   └── utils/            # Utility functions
├── src/              # Source files
│   ├── main.c            # Game entry point
│   ├── puissance4API.c    # Game implementation
│   ├── audio.c           # Audio implementation
│   └── utils/            # Utility implementations
├── firstparty/       # First-party APIs
│   └── APIs/             # Shared game APIs
├── docs/             # Documentation
├── Doxyfile.min     # Minimal Doxygen configuration
└── Makefile         # Build configuration
```

## Building

To build the game, run:

```bash
make
```

To clean build artifacts:

```bash
make clean
```

## Documentation

Documentation is generated via Doxygen. To generate:

```bash
doxygen Doxyfile.min
# or
doxygen docs/Doxyfile
```

Generated documentation will be available in the `docs/html/` directory.

## API Usage

```c
#include "puissance4API.h"

// Initialize game
Puissance4Game_St* game = puissance4_initGame(NULL);
if (!game) {
    // Handle error
}

// Main game loop
while (puissance4_isRunning(game)) {
    puissance4_gameLoop(game);
}

// Cleanup
puissance4_freeGame(game);
```

## Author

- **Maxime CHAUVEAU** - *Initial development* - March 2026

## License

This project is part of the Multi-Mini-Games framework.