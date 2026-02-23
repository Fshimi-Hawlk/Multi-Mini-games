# CHANGELOG.md

All notable changes to this sub-project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/),
and this sub-project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html)
(or Calendar Versioning where appropriate).

## [Unreleased]

### Added

- **Modular mini-game system**: `MiniGame_St` structure for dynamic game loading
- **Solitaire integration**: Full support for launching Solitaire from lobby
- **Graceful degradation**: Lobby starts even if mini-game APIs fail to initialize
- **Game availability check**: Each game's API is tested at startup
- **Portal visualization**: Colored rectangles with game names displayed in 2D world

### Changed

- **API pattern**: All mini-games now use standardized API (`initGame`, `gameLoop`, `freeGame`, `isRunning`)
- **Memory management**: Game instances allocated/freed dynamically per game session
- **Scene management**: Simplified switch-case with `GameScene_Et` enum

### Fixed

- Proper cleanup of game instances when returning to lobby
- Window size restoration after exiting a mini-game

### Removed

- Hardcoded Tetris references (replaced with modular system)
