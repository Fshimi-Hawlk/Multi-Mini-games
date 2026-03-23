@page root_makefile Root Makefile Documentation

@ref index "Back to Home"

**Last checked against project structure:** March 16, 2026  

This page describes the **root Makefile** in the Multi Mini-Games monorepo.

The root Makefile is responsible for:
- Building static libraries for all integrated games (`libtetris.a`, etc.)
- Copying each game's public API header (`tetrisAPI.h`, etc.) into `firstparty/APIs/`
- Linking everything together into one single lobby executable
- Doing all of this incrementally when possible (only rebuild what's changed)

It supports the same build modes as the sub-project Makefiles so the whole monorepo feels consistent.

## Build Modes

You choose the mode by adding `MODE=xxx` to any make command. Default is `release`.

| Mode              | Compiler | Main flags                             | Extra notes / requirements    |
|-------------------|----------|----------------------------------------|-------------------------------|
| release           | gcc      | -O2                                    | Optimized, no debug symbols   |
| debug             | gcc      | -Wall -Wextra -g -O0                   | Basic debug + warnings        |
| strict-debug      | gcc      | -Werror -Wall -Wextra -pedantic -g     | Warnings become errors        |
| clang-debug       | clang    | + -fsanitize=address,undefined         | Needs clang installed         |
| valgrind-debug    | gcc      | -g -O0 (valgrind run wrapper)          | Needs valgrind installed      |

## Main Targets

- `make` / `all` / `bin`  
  Builds/updates libraries + links the lobby executable (incremental)

- `libs`  
  Builds or updates only the static libraries + copies API headers

- `rebuild`  
  Full clean + rebuild everything from scratch

- `clean`  
  Removes the root `build/` folder (libraries + exe)

- `clean-all`  
  Cleans root + calls clean in every sub-project

- `rebuild-exe`  
  Forces relink of the lobby executable only (useful after lib changes)

- `run-exe`  
  Runs the lobby from `build/bin/main` (or whatever MAIN_NAME is set to)

- `tests` / `run-tests`  
  Builds and runs all unit tests (lobby + every game)

- `help`  
  Prints this list of targets + modes

## Common Commands (what we use most)

Normal quick build + run:

```bash
make bin
make run-exe
```

Force everything in debug with sanitizers (good for catching bugs):

```bash
make MODE=clang-debug rebuild run-exe
```

Rebuild libraries only (after adding/changing a game):

```bash
make rebuild-libs
```

Run all tests with extra checks:

```bash
make MODE=clang-debug run-tests
```

## Asset Path Handling

To make assets work both standalone and in the lobby:

- When building inside a game folder -> uses `assets/…`
- When building from root -> uses `<gamename>/assets/…` (e.g. `tetris/assets/`)

This is controlled by passing `-DASSET_PATH="..."` via `EXTRA_CFLAGS` in the Makefile.

## API Header Copying

During `libs`, `bin`, `rebuild-libs` targets:
- Each game's `<gamename>API.h` is copied from `<gamename>/include/` to `firstparty/APIs/`
- Only copies if the header actually exists

This lets the lobby `#include "APIs/tetrisAPI.h"` without path mess.

## Debugging & Logs

- Tests create folders like `logs/tests-2026-03-16_14-30/`
- Valgrind mode creates `logs/valgrind-<timestamp>/`
- Add `VERBOSE=1` to any make command to see full gcc/clang lines

## Generating Documentation

```bash
make docs
```

-> `open docs/doxygen/index.html`


**Created:** March 02, 2025  
**Last updated:** March 16, 2026  
**Author:** [Fshimi Hawlk](https://github.com/Fshimi-Hawlk)

@ref index "Back to Home"