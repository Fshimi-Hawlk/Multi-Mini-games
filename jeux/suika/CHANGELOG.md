# CHANGELOG.md

All notable changes to this project will be documented in this file.

## [Unreleased]

### Added
- Complete Suika game implementation
- Fruit physics with gravity, collisions, and rotation
- Fruit merging system: identical fruits combine into larger fruits
- 11 fruit types from Cherry to Watermelon
- Score system based on fruit merge points
- Game over detection when fruits exceed the drop line
- High score tracking for the session
- Sprite atlas loading for fruit textures
- Smooth visual rendering with shadows
- Gradient background and container UI
- HUD with score display and instructions
- Standalone mode support via main.c
- Lobby integration via suikaAPI.h

### Changed
- Moved dropTimer from static variable to game state struct for proper reset behavior
- Applied consistent code style (braces on new lines)

### Fixed
- Fixed static dropTimer persisting across game resets

### Removed
- (nothing yet)

## Credits

**Last updated: February 24, 2026**  
**Author: Maxime CHAUVEAU**
