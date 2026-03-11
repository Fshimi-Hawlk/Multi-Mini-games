# CHANGELOG.md

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html)
(or Calendar Versioning where appropriate).

## How to update this file

- Only *important* changes go here (new game started, big shared code added, lobby progress, big refactor, etc.)
- We write in the **[Unreleased]** section until we decide it's time for a version (for example 0.1.0 when lobby + first few games work together)

## [Unreleased]

### Added

- Initial monorepo structure with shared folders (`firstparty/`, `thirdparty/`, `assets/`).
- Root documentation files: README.md, CONTRIBUTING.md, TODO.md, CodeStyleAndConventions.md
- Example sub-project folder (`sub-project_example/`) showing the structure we want for each game
- Basic legal files: LICENSE, NOTICE (for third-party code credits)
- Root-level Makefile: supports building merged games' static libraries lazily, copying API headers to `firstparty/APIs/`, incremental lobby linking
- **Global error system**: `firstparty/APIs/gameError.h` with `GameError_Et` enum for standardized error codes across all mini-games
- **Global configuration system**: `firstparty/APIs/gameConfig.h` with `GameConfig_St`, `AudioConfig_St`, `VideoConfig_St` and macros for default values
- **Solitaire game** by Maxime CHAUVEAU: Complete implementation with drag-and-drop, scoring, timer, win detection
- **Lobby integration**: Modular mini-game launcher with `MiniGame_St` structure for easy game addition

### Changed

- Updated root README with clearer explanations and Git commands suited for beginners
- Rewrote CONTRIBUTING.md with simplified workflow explanations, beginner-friendly Git command examples, and softer tone regarding code style consistency
- Updated CHANGELOG.md itself to better explain update rules and reflect documentation improvements
- Revised TODO.md wording to be more straightforward while keeping the task list intact
- Reworded CodeStyleAndConventions.md to clearly state it is a preferred style (not strict law), with invitation to discuss points that feel inconvenient
- **API standardization**: All mini-game APIs now follow the same pattern (`*_initGame`, `*_gameLoop`, `*_freeGame`, `*_isRunning`)
- **Solitaire renderer**: Improved card display with proper scaling, empty slot placeholders, and animated ace symbols
- **Solitaire drag & drop**: Multiple card selection from tableau piles, centered card on mouse cursor
- **Solitaire documentation**: Bilingual documentation (English/French) in `docs/doxygen/`

### Fixed

- Solitaire texture loading: Fixed relative paths for assets when running from build directory
- Solitaire memory management: Assets now loaded once at init, not every frame
- Lobby gracefully handles unavailable mini-games (API returning NULL)
- **Solitaire stock pile**: Fixed empty stock display (rectangle only, no animated ace)
- **Solitaire card shuffle**: Added srand() initialization for random card dealing
- **Solitaire click zones**: Fixed collision detection for stock pile recycling
- **Solitaire drag rendering**: Fixed double card display when dragging
- **Solitaire pile drag**: Cards above dragged card now follow correctly
- **Solitaire card removal**: Fixed removal of correct number of cards from source pile

### Removed

- (none yet)

## [0.1.0] - 2026-02-21

### Added

- Initial release with Solitaire (Klondike) game and Lobby

## Credits

**Created: January 15, 2025**  
**Last updated: March 03, 2026**  
**Author: Fshimi Hawlk, Maxime CHAUVEAU**
