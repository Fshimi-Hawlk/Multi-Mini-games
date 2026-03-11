# CHANGELOG.md

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## How to update this file

- Only *important* changes go here (bug fixes, new features, major refactors, etc.)
- Write in the **[Unreleased]** section until a version is released

## [Unreleased]

### Added

- **Win detection**: Automatic victory detection when all cards are in foundation piles (by Maxime CHAUVEAU)
- **Lose detection**: Automatic defeat detection every 2 minutes checking for available moves (by Maxime CHAUVEAU)
- **Drag and drop**: Multiple card selection and movement from tableau piles (by Maxime CHAUVEAU)
- **Random shuffle**: Fisher-Yates shuffle algorithm for card dealing (by Maxime CHAUVEAU)

### Changed

- **Stock pile rendering**: Simplified to show only empty slot when stock is empty (by Maxime CHAUVEAU)
- **Card rendering**: Fixed double card display issue when dragging cards (by Maxime CHAUVEAU)
- **Drag offset**: Cards now centered on mouse cursor when dragging (by Maxime CHAUVEAU)
- **Waste pile click zone**: Fixed collision detection to use unique rectangle for stock recycling (by Maxime CHAUVEAU)

### Fixed

- **Stock pile bug**: Empty stock now shows rectangle only, not animated ace symbols (by Maxime CHAUVEAU)
- **Card distribution**: Fixed random shuffle - added srand() initialization (by Maxime CHAUVEAU)
- **Drag from middle of pile**: Cards above the dragged card now follow correctly (by Maxime CHAUVEAU)
- **Card removal on drop**: Fixed removal of correct number of cards from source pile (by Maxime CHAUVEAU)
- **Card flip**: Cards are now properly flipped face-up after moving (by Maxime CHAUVEAU)

### Removed

- (none yet)

## [1.0.0] - 2026-02-21

### Added

- Initial Solitaire (Klondike) implementation (by Maxime CHAUVEAU)
- Card sprites and back design (by Maxime CHAUVEAU)
- Drag and drop basic functionality (by Maxime CHAUVEAU)
- Score and timer system (by Maxime CHAUVEAU)
- Win screen with congratulations (by Maxime CHAUVEAU)

### Changed

- (none)

### Fixed

- (none)

## Credits

**Created: February 21, 2026**  
**Last updated: March 03, 2026**  
**Author: Maxime CHAUVEAU**
