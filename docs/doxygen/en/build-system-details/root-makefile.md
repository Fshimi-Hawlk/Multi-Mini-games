@page root_makefile Root Makefile Documentation

This file documents the **root-level Makefile** of the Multi Mini-Games monorepo.

It allows building all merged games' static libraries, copying their API headers to `firstparty/APIs/`, and linking everything into the lobby executable - with lazy/incremental behavior whenever possible.

## Overview

- Builds static libraries (`lib<gamename>.a`) from each module folder (`tetris/`, `block-blast/`, etc.)  
- Copies each module's `<gamename>API.h` -> `firstparty/APIs/`  
- Links all libraries into the lobby executable  
- Supports the same build modes as sub-project Makefiles  
- Output locations:  
  - Libraries: `build/lib/lib<gamename>.a`  
  - Lobby executable: `build/bin/<MAIN_NAME>` (default: `main`)  

## Supported Build Modes

Same as sub-projects:

| Mode              | Compiler | Key flags                              | Requirements       |
|-------------------|----------|----------------------------------------|--------------------|
| `release`         | gcc      | `-O2`                                  | -                  |
| `debug`           | gcc      | `-Wall -Wextra -g -O0`                 | -                  |
| `strict-debug`    | gcc      | `-Werror -Wall -Wextra -pedantic -g`   | -                  |
| `clang-debug`     | clang    | + `-fsanitize=address,undefined`       | Clang              |
| `valgrind-debug`  | gcc      | + `-g -O0`                             | Valgrind           |

**Note**: Use `MODE=...` in commands, e.g., `make MODE=debug bin`.

## Main Targets

- `all` / `bin`: Default - build libraries + lobby executable (incremental)
- `libs`: Build all game static libraries (lazy)
- `rebuild`: Full clean + build everything
- `clean`: Remove build artifacts
- `help`: Show all targets and modes

## Common Commands & Examples

Normal development flow (fastest):

```bash
make bin                # incremental build
make rebuild-exe        # force relink lobby only
make run-exe            # run
```

One liner:

```bash
make MODE=clang-debug rebuild run-exe
```

Full reset:

```bash
make rebuild
```

Only force-rebuild libraries (no lobby):

```bash
make rebuild-libs
```

Test everything:

```bash
make run-tests
make MODE=clang-debug run-tests
```

## Asset Path Handling

Each module sets its own asset prefix at compile time:  

- Standalone build (inside game folder): `assets/...`  
- Root build: `<module-name>/assets/...` (e.g. `tetris/assets/...`)  

This is done via `-DASSET_PATH="..."` passed through EXTRA_CFLAGS.  

## API Headers

When building libraries (`libs`, `bin`, `rebuild-libs`):

- Each module's `<module-name>API.h` (e.g. `tetrisAPI.h`) is copied from `<module>/include/` to `firstparty/APIs/`
- Copy happens only if the file exists

## Logging & Debugging

- `run-tests` creates timestamped folders in `logs/tests-<timestamp>/`
- In `valgrind-debug` mode: also creates `logs/valgrind-<timestamp>/`
- Use `VERBOSE=1` to see full commands

### Portability / Requirements

- `clang-debug` -> needs `clang`
- `valgrind-debug` -> needs `valgrind`
- `cmp` (used to skip unchanged files) -> almost always present on Unix-like systems

**Created:** March 02, 2026  
**Author:** [Fshimi Hawlk](https://github.com/Fshimi-Hawlk)