# CHANGELOG.md

All notable changes to this sub-project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/),
and this sub-project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html)
(or Calendar Versioning where appropriate).

## [Unreleased]

### Added
- Initial lobby implementation with platformer-style navigation
- Player physics: gravity, jumping, horizontal movement
- Platform collision detection and resolution
- Game zone detection for mini-game transitions
- Scene management system for switching between lobby and games
- Font loading system with multiple sizes
- Skin selection UI framework
- Shared globals for window dimensions and resources

### Changed
- Refined player collision resolution for smoother platforming
- Optimized rendering with proper draw ordering
- Improved code style consistency (braces on new lines)

### Fixed
- Fixed potential division by zero in `randint()` utility function
- Fixed broken `vec2Mul` and `vec2Scale` macros (missing `*` operator)
- Fixed `get_caller_info()` in non-debug logger builds

### Removed
- (nothing yet)

## Credits

**Last updated: February 24, 2026**  
**Author: [Fshimi Hawlk](https://github.com/Fshimi-Hawlk)**
