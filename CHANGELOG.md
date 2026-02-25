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
- Basic legal files: LICENSE
- **Configuration system**: `systemSettings.h` with `AudioSettings_St`, `VideoSettings_St`, `SystemSettings_St` and default macros
- **Suika Game**: Complete implementation with physics, fruit merging, scoring system
- **Suika API**: `suikaAPI.h` following the standard mini-game pattern

### Changed
- Updated root README with clearer explanations and Git commands suited for beginners
- Rewrote CONTRIBUTING.md with simplified workflow explanations, beginner-friendly Git command examples, and softer tone regarding code style consistency
- Updated CHANGELOG.md itself to better explain update rules and reflect documentation improvements
- Revised TODO.md wording to be more straightforward while keeping the task list intact
- Reworded CodeStyleAndConventions.md to clearly state it is a preferred style (not strict law), with invitation to discuss points that feel inconvenient
- **Extended error codes**: Added `ERROR_INVALID`, `ERROR_INVALID_SETTING`, `ERROR_AUDIO_LOAD`, `ERROR_WINDOW_INIT`, `ERROR_ASSET_LOAD` to `generalAPI.h`
- **Standardized API pattern**: All mini-game APIs follow consistent naming and return `Error_Et` codes

### Fixed
- Fixed broken macros `vec2Mul` and `vec2Scale` in lobby utils.h (missing `*` operator)
- Fixed division by zero in `randint()` when max < min
- Fixed `get_caller_info()` stub in non-debug builds (was not writing to buffer)
- Fixed static `dropTimer` in Suika that persisted across game resets
- Fixed missing `logger.h` include in `systemSettings.h` implementation section
- Added missing `srand()` call in Suika API initialization

### Removed
- (none yet)

## Credits

**Last updated: February 24, 2026**  
**Author: [Fshimi Hawlk](https://github.com/Fshimi-Hawlk)**
