# CHANGELOG.md

All notable changes to this sub-project (Block Blast reproduction + extensions) will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/),
and this sub-project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html)
(or Calendar Versioning where it makes more sense for a student project).

## [Unreleased]

### Added
- Core gameplay foundation: 8×8 grid, shape dragging & placement with snap-to-grid, row/column clearing detection
- Random shape generation: bell-curve weighted distribution + prefab bag system (classic polyominoes + variants)
- Basic scoring system with streak/combo tracking and on-screen text display
- Visual feedback: streak counter, score display, shape snapping back if placement invalid
- Custom font support (Nutino-Black) for nicer text rendering
- Adjustable weights for shape generation to control difficulty/variety
- Arena-based memory management (from nob) for easier allocations

### Changed
- Major documentation push: 
    tons of Doxygen-style comments
    README with gameplay overview, goals, planned features
    CHANGELOG with the commit messages (`git log --pretty=format:%B`)
- Refactored shape placement logic, slot shuffling, weight adjustment for cleaner future extensions
- Improved Doxyfile: better main page (README as entry), less redundant output, faster generation

### Fixed
- Shape placement grid-snapping bugs
- Clamp macro name collision
- Float precision in ARRAY_LEN macro
- Missing fail guards in critical functions

### Removed
- Unused compile_commands target
- Redundant clean-obj target
- Temporary stringView lib experiment
- Doxygen-generated files from .gitignore (so we can commit docs when ready)