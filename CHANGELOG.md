# CHANGELOG.md

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html)
(or Calendar Versioning where appropriate).

## [Unreleased]

### Added
- Initial monorepo structure with shared folders (`firstparty/`, `thirdparty/`, `assets/`).  
**Note**: `firstparty` is like `thirdparty`, but the content is made by someome of the team.
- Root-level files: [README.md](README.md), [TODO.md](TODO.md), [CONTRIBUTING.md](CONTRIBUTING.md) + [Code Style & Conventions](CodeStyleAndConventions.md).
- Legal stuff: [LICENSE](LICENSE), [NOTICE](thirdparty/NOTICE)
- Sub-project folder example:
    - Folder Structure (src, include, tests, docs)
    - [Makefile](sub-project-example/Makefile)
    - [Makefile documentaion](sub-project-example/makefile.md)
    - [Minimal Doxygen Configuration File](sub-project-example/Doxyfile.min)
    - [Example README.md at Sub-Project Level](sub-project-example/README.md)

### Changed
- Updated root README with workflow and structure details.

### Removed
- (none yet)