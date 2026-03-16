@page sub_project_makefile Sub-Project Makefile Documentation

@ref index "Back to Home"

**Last checked against project structure:** March 16, 2026

This page describes the Makefile used inside each sub-project folder (tetris/, lobby/, new games copied from sub-project-example/, etc.).

The Makefile handles building the game as a standalone executable for testing, producing a static library for lobby integration, running unit tests, and supporting multiple debug/optimization modes.

## Overview

Main responsibilities:
- Build standalone executable from `src/main.c` (for quick testing)
- Build static library `lib<gameName>.a` (used by root Makefile to link into lobby)
- Build and run unit tests from `tests/`
- Support several build modes (release / debug / sanitizers / valgrind)
- Automatic header dependency tracking (-MMD -MP)
- Silent by default, verbose with VERBOSE=1
- Custom flags via EXTRA_CFLAGS / EXTRA_LDFLAGS
- Log test and valgrind output in timestamped folders under `logs/`

Typical output paths (relative to sub-project folder):
- Objects: `build/obj/`
- Standalone binary: `build/bin/main`
- Static lib: `build/lib/lib<gameName>.a`
- Test binaries: `build/bin/tests/<test-name>`

## Supported Build Modes

| Mode            | Compiler | Main flags                               | Requirements       | Typical use case                       |
|-----------------|----------|------------------------------------------|--------------------|----------------------------------------|
| release         | gcc      | -O2                                      | -                  | Final / optimized standalone runs      |
| debug           | gcc      | -Wall -Wextra -g -O0                     | -                  | Normal development                     |
| strict-debug    | gcc      | -Werror -Wall -Wextra -pedantic -g -O0   | -                  | Catch warnings before merge            |
| clang-debug     | clang    | same + -fsanitize=address,undefined      | clang installed    | Memory / UB bug hunting                |
| valgrind-debug  | gcc      | -Werror -Wall -Wextra -pedantic -g -O0   | valgrind installed | Runtime leak / invalid read checks     |

Default mode is `release`.

## Most Useful Targets

- `all` / default target   : build standalone executable
- `static-lib`             : build static library for lobby integration
- `tests`                  : build all test binaries
- `run-main`               : build + run standalone executable
- `run-tests`              : build + run all tests (with live output + logs)
- `rebuild`                : clean + all
- `clean`                  : remove build/ folder
- `run-gdb`                : run standalone binary under gdb

## Quick Examples

Basic standalone build + run:

```bash
make
make run-main
```

Debug / strict mode:

```bash
make MODE=debug
make MODE=strict-debug rebuild run-main
```

With sanitizers or Valgrind:

```bash
make MODE=clang-debug run-main
make MODE=valgrind-debug run-tests
```

Build only the static lib (for lobby):

```bash
make static-lib
```

Verbose + extra warnings:

```bash
make VERBOSE=1 EXTRA_CFLAGS="-Wshadow -Wconversion" MODE=strict-debug
```

## Logging & Output Notes

- `run-tests` creates `logs/tests-<timestamp>/` with one .log file per test (stdout + stderr)
- In `valgrind-debug` mode: extra `logs/valgrind-<timestamp>/` folders with Valgrind reports
- Logs only save non-empty output
- Crash output reliability: uses `stdbuf --output=L --error=L` (line buffering) when available so last printf() lines show up even on SIGABRT or ASan crash
- If `stdbuf` is missing → warning printed once, output might truncate on crash → add explicit `fflush(stdout); fflush(stderr);` in test code if needed

## Requirements & Portability

- `clang-debug` needs clang
- `valgrind-debug` needs valgrind
- `stdbuf` (from coreutils) is optional but recommended for clean test output on Linux

## Related Pages

@ref sub_project_internal_makefile_structure "Makefile Internal Structure": How the Makefile is **internally structured**

**Created:** March 16, 2026  
**Author:** [Fshimi Hawlk](https://github.com/Fshimi-Hawlk)

@ref index "Back to Home"