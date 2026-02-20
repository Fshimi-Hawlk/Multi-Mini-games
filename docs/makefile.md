# Root Makefile Documentation

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
| `valgrind-debug`  | gcc      | `-g -O0` + Valgrind wrapper at runtime | Valgrind           |

Default: `release`

## Key Root-Level Targets

`all` / `make`     -> Build libraries + lobby executable
`rebuild`          -> Clean + rebuild everything
`clean`            -> Remove root-level build/ folder only
`clean-all`        -> Remove root build/ + clean every submodule
`clean-libs`       -> Clean only libraries (root + subdirs)
`clean-exe`        -> Clean only lobby executable
`libs`             -> Build/update static libraries + copy API headers
`rebuild-libs`     -> Clean + force rebuild all libraries + copy API headers
`bin`              -> Build lobby executable (incremental, depends on libs)
`rebuild-exe`      -> **Force** rebuild lobby executable only
`run-exe`          -> Run the lobby executable from `build/bin`
`tests`            -> Build test binaries in all modules + lobby
`rebuild-tests`    -> Clean + rebuild tests
`run-tests`        -> Run all tests and show summary
`help`             -> Print an help message

## Common Commands

```bash
# Normal development flow (fastest)
make bin                # incremental build
make rebuild-exe        # force relink lobby only
make run-exe            # run

# Full reset
make rebuild

# Only force-rebuild libraries (no lobby)
make rebuild-libs

# Test everything
make run-tests
make MODE=clang-debug run-tests
```

## Asset Path Handling

Each module sets its own asset prefix at compile time:  

- Standalone build (inside game folder): `assets/...`  
- Root build: `<modulename>/assets/...` (e.g. `tetris/assets/...`)  

This is done via `-DASSET_PATH="..."` passed through EXTRA_CFLAGS.  

## API Headers

When building libraries (`libs`, `bin`, `rebuild-libs`):

- Each module's `<modulename>API.h` (e.g. `tetrisAPI.h`) is copied from `<module>/include/` to `firstparty/APIs/`
- Copy happens only if the file exists

## Logging & Debugging

- `run-tests` creates timestamped folders in `logs/tests-<timestamp>/`
- In `valgrind-debug` mode: also creates `logs/valgrind-<timestamp>/`
- Use `VERBOSE=1` to see full commands

### Portability / Requirements

- `clang-debug` -> needs `clang`
- `valgrind-debug` -> needs `valgrind`
- `cmp` (used to skip unchanged files) -> almost always present on Unix-like systems

## Author

[Fshimi Hawlk](https://github.com/Fshimi-Hawlk)