# CHANGELOG.md

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html)
(or Calendar Versioning where it makes more sense).

## How to update this file

- Only *important / visible* changes go here (new game branch merged, big shared code added, lobby started, major refactor, documentation overhaul, etc.)
- We add stuff under **[Unreleased]** until we feel like cutting a version tag (for example 0.1.0 when lobby + first 2–3 games actually work together)
- Try to keep entries short and grouped by type (Added / Changed / Fixed / Removed)

## [Unreleased]

### Added
- Initial monorepo structure with shared folders (`firstparty/`, `thirdparty/`, `assets/`)
- Root documentation files: README.md, CONTRIBUTING.md, TODO.md, CodeStyleAndConventions.md, .gitignore, LICENSE, NOTICE (third-party credits)
- `sub-project-example/` folder as the template for every mini-game (includes Makefile, makefile.md, Doxyfile.min, CHANGELOG.md stub, README template)

### Changed
- Improved root README.md with clearer explanations + beginner-friendly Git examples
- Rewrote CONTRIBUTING.md to be softer / more welcoming + added simple Git workflow + commit message examples
- Updated this CHANGELOG.md file itself to explain how we want to use it
- Reworded TODO.md to make tasks clearer without changing priorities
- Softened tone in CodeStyleAndConventions.md — made it clear it's a *recommended* baseline, not strict law, and invited discussion

### Fixed
- (nothing yet)

### Removed
- (nothing yet)