@page changelog Project Changelog

# CHANGELOG

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html)
(or Calendar Versioning where appropriate).

@section update_rules How to update this file

- Only @b important changes go here (new game started, big shared code added, lobby progress, big refactor, etc.)
- We write in the @b [Unreleased] section until we decide it's time for a version (for example 0.1.0 when lobby + first few games work together)

@section unreleased [Unreleased]

@subsection added Added
- Initial monorepo structure with shared folders (@c firstparty/, @c thirdparty/, @c assets/).
- Root documentation files: README.md, CONTRIBUTING.md, TODO.md, CodeStyleAndConventions.md
- Example sub-project folder (@c sub-project-example/) showing the structure we want for each game
- Basic legal files: LICENSE, NOTICE (for third-party code credits)
- Root-level Makefile: supports building merged games' static libraries lazily, copying API headers to @c firstparty/APIs/, incremental lobby linking
- @b Configuration system: @c gameConfig.h with @c GameConfig_St, @c AudioConfig_St, @c VideoConfig_St and default macros
- @b Solitaire API: @c solitaireAPI.h following the standard mini-game pattern (by Maxime CHAUVEAU)
- @b Tetris API stub: @c tetrisAPI.h for future implementation

@subsection changed Changed
- Updated root README with clearer explanations and Git commands suited for beginners
- Rewrote CONTRIBUTING.md with simplified workflow explanations, beginner-friendly Git command examples, and softer tone regarding code style consistency
- Updated CHANGELOG.md itself to better explain update rules and reflect documentation improvements
- Revised TODO.md wording to be more straightforward while keeping the task list intact
- Reworded CodeStyleAndConventions.md to clearly state it is a preferred style (not strict law), with invitation to discuss points that feel inconvenient
- @b Extended error codes: Added @c ERROR_INVALID_CONFIG, @c ERROR_AUDIO_LOAD, @c ERROR_WINDOW_INIT, @c ERROR_ASSET_LOAD to @c generalAPI.h
- @b Standardized API pattern: All mini-game APIs now use @c GameConfig_St* for configuration, added @c isRunning() function, and follow consistent naming

@subsection fixed Fixed
- (none yet)

@subsection removed Removed
- (none yet)

**Last updated:** March 02, 2026  
**Author:** [Fshimi Hawlk](https://github.com/Fshimi-Hawlk)