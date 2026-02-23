# Solitaire (Klondike)

**Classic Klondike Solitaire Card Game**

## Overview

A fully-featured Klondike Solitaire implementation with drag-and-drop card movement, scoring system, and timer.

### Core Gameplay

- **Objective**: Move all cards to the four foundation piles, sorted by suit from Ace to King
- **Tableau**: Build down in alternating colors (red on black, black on red)
- **Stock**: Draw cards from the stock pile to the waste pile
- **Controls**:
  - **Left Click**: Drag cards between piles
  - **Click Stock**: Draw a card (or recycle waste pile when empty)
  - **N Key**: Start a new game

### Features

- Drag and drop multiple cards at once
- Score tracking (+10 for foundation moves, +5 for tableau moves)
- Game timer
- Win detection with congratulations screen
- Animated card back symbols when piles are empty

## Tech / Structure Notes

- **Language**: C (C99)
- **Graphics**: Raylib 5.5
- **Folders**:
  - `src/core/`: Game logic
  - `src/ui/`: Rendering
  - `src/utils/`: Helper functions
  - `include/`: Public headers
  - `assets/solitaire/`: Card textures

## Building

```bash
cd solitaire
make            # Build executable
make static-lib # Build static library for lobby
make run-main   # Build and run
```

## API Usage

```c
#include "APIs/solitaireAPI.h"

SolitaireGame_St* game = solitaire_initGame(NULL);
while (solitaire_isRunning(game)) {
    solitaire_gameLoop(game);
}
solitaire_freeGame(game);
```

## Author

Maxime CHAUVEAU - February 2026
