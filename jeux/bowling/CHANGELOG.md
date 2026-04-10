# CHANGELOG.md

All notable changes to this project will be documented in this file.

## [Unreleased]

### Added
- Initial game implementation
- Core gameplay mechanics
- User interface and controls
- Physics system (if applicable)
- Scoring system (if applicable)

### Changed
- (nothing yet)

### Fixed
- (nothing yet)

### Removed
- (nothing yet)

## [3.0] - 2026-03-16

### Added
- Raylib3D support for enhanced 3D graphics
- Complete 3D environment decoration (walls, pillars, ceiling, decorative elements)
- Physics module (physics.c, physics.h) - separated physics logic from game code
- Additional environment details (seats, pillars, score display structure)

### Changed
- Refactored bowlingAPI.c to use physics module
- Improved 3D rendering with environment decoration
- Enhanced visual experience with complete bowling alley atmosphere

### Author
- **Maxime Chauveau** - Initial work

## [2.0] - 2026-02-25

### Added
- Complete bowling game implementation
- Ball physics with spin mechanics
- Pin collision and tumble physics
- Full 10-frame scoring system
- Dynamic camera system
- Visual effects and particle system
- End game statistics
