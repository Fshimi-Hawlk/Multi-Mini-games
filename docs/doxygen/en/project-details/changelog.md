@page changelog Changelog

@ref index "Back to Home"

**Last checked against project structure:** March 16, 2026

This page lists all notable changes to the Multi Mini-Games monorepo.

The format follows [Keep a Changelog](https://keepachangelog.com/en/1.1.0/) conventions.  
We use Semantic Versioning where it makes sense, or Calendar Versioning for early development stages.

### How we maintain this changelog

- Only significant changes are recorded here (new game integration, major shared code, big refactors, important documentation updates, etc.)
- Everything goes under **[Unreleased]** until the team decides a version tag is ready (example: 0.1.0 when lobby + several games are playable together)

### [Unreleased]

#### Added
- Monorepo foundation: shared folders (`firstparty/`, `thirdparty/`, `assets/`)
- Core documentation files in root: README.md, CONTRIBUTING.md, TODO.md, CodeStyleAndConventions.md
- Template folder `sub-project-example/` to guide new game structure
- Legal basics: LICENSE and NOTICE files (third-party credits)
- Root Makefile with lazy static-lib builds, API header copying to `firstparty/APIs/`, and incremental lobby executable linking
- Central configuration system: `gameConfig.h` defining `GameConfig_St`, `AudioConfig_St`, `VideoConfig_St` + default macros
- First full game API: `solitaireAPI.h` (standard mini-game pattern, implemented by Maxime CHAUVEAU)
- Initial Tetris API header: `tetrisAPI.h` (stub for upcoming implementation)

#### Changed
- Improved root README: clearer project explanation + beginner-friendly Git commands
- Simplified and softened CONTRIBUTING.md: easier workflow steps, practical Git examples, less strict tone on style
- This changelog itself: added clearer update rules and reflected recent doc improvements
- Made TODO.md wording more direct while preserving the full task list
- Updated CodeStyleAndConventions.md: now presented as a recommended style (not mandatory), with invitation to discuss inconvenient rules
- Extended error handling: new codes `ERROR_INVALID_CONFIG`, `ERROR_AUDIO_LOAD`, `ERROR_WINDOW_INIT`, `ERROR_ASSET_LOAD` added to `generalAPI.h`
- Standardized mini-game API pattern: consistent use of `GameConfig_St*`, added `isRunning()` helper, unified naming across games

#### Fixed
- (none recorded yet)

#### Removed
- (none recorded yet)

**Created:** January 15, 2025  
**Last updated:** March 16, 2026  
**Author:** [Fshimi Hawlk](https://github.com/Fshimi-Hawlk)

@ref index "Back to Home"