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
- Example sub-project folder (`sub-project-example/`) showing the structure we want for each game
- Basic legal files: LICENSE, NOTICE (for third-party code credits)
- Root-level Makefile: supports building merged games' static libraries lazily, copying API headers to `firstparty/APIs/`, incremental lobby linking
- **Configuration system**: `gameConfig.h` with `GameConfig_St`, `AudioConfig_St`, `VideoConfig_St` and default macros
- **Solitaire API**: `solitaireAPI.h` following the standard mini-game pattern (by Maxime CHAUVEAU)
- **Tetris API stub**: `tetrisAPI.h` for future implementation
- **`.gitignore` update**: Added rule to ignore Windows Zone Identifier files (`:Zone.Identifier`) (by Maxime CHAUVEAU)

### Changed
- Updated root README with clearer explanations and Git commands suited for beginners
- Rewrote CONTRIBUTING.md with simplified workflow explanations, beginner-friendly Git command examples, and softer tone regarding code style consistency
- Updated CHANGELOG.md itself to better explain update rules and reflect documentation improvements
- Revised TODO.md wording to be more straightforward while keeping the task list intact
- Reworded CodeStyleAndConventions.md to clearly state it is a preferred style (not strict law), with invitation to discuss points that feel inconvenient
- **Extended error codes**: Added `ERROR_INVALID_CONFIG`, `ERROR_AUDIO_LOAD`, `ERROR_WINDOW_INIT`, `ERROR_ASSET_LOAD` to `generalAPI.h`
- **Standardized API pattern**: All mini-game APIs now use `GameConfig_St*` for configuration, added `isRunning()` function, and follow consistent naming

### Fixed
- (none yet)

### Removed
- (none yet)

## Credits

**Created: January 15, 2025**  
**Last updated: March 17, 2026**  
**Authors:**
- [Fshimi Hawlk](https://github.com/Fshimi-Hawlk)
- Maxime CHAUVEAU