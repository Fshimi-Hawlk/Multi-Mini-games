# Makefile Documentation

This file documents the project's main Makefile.
It is a comprehensive build script supporting multiple compilation modes, automatic dependency tracking, separate test binaries, logging, and runtime tools like Valgrind.

## Overview

The Makefile manages:
- Building the **Server executable** from `src/server.c` and `src/lobby.c`.
- Building the **Static Library** (`libreseau.a`) for client integrations.
- Linking shared library objects to both main and tests (avoids multiple-definition errors).
- Multiple build modes: release, basic-debug, strict-debug, clang-debug (sanitizers), valgrind-debug.
- Automatic header dependency tracking (`-MMD -MP`) — disable with `NO_DEPENDENCY_TRACKING=1`.
- Silent builds by default — enable verbose output with `VERBOSE=1`.
- Custom flags via `EXTRA_CFLAGS` / `EXTRA_LDFLAGS`.
- Logging for tests and valgrind runs in timestamped folders under `logs/`.

Output locations:
- Object files: `build/obj/`
- Server binary: `build/bin/server`
- Static Library: `build/lib/libreseau.a`
- Test binaries: `build/bin/tests/<test-name>`

## Supported Build Modes

| Mode             | Compiler | Flags / Features                              | Requirements          | Notes                                      |
|------------------|----------|-----------------------------------------------|-----------------------|--------------------------------------------|
| `release`        | gcc      | `-O2`                                         | None                  | Optimized, no debug info                   |
| `basic-debug`    | gcc      | `-Wall -Wextra -g -O0`                        | None                  | Basic warnings + debug symbols             |
| `strict-debug`   | gcc      | `-Werror -Wall -Wextra -pedantic -g -O0`      | None                  | Strict warnings, no sanitizers             |
| `clang-debug`    | clang    | `-Werror -pedantic -fsanitize=address,undefined` | Clang installed       | Compile-time sanitizers (ASan + UBSan)     |
| `valgrind-debug` | gcc      | `-Wall -Wextra -g -O0`                        | Valgrind installed    | Debug build + runtime Valgrind checks      |

Default: `release`

## Key Targets

- `all`               → Build both **Server** and **Library** (default target).
- `server`            → Build only the server executable (`build/bin/server`).
- `lib`               → Build only the static library (`build/lib/libreseau.a`).
- `tests`             → Build all test executables.
- `run-tests`         → Build + run all tests (live stdout + per-test logs in `logs/tests-<timestamp>/`).
- `rebuild`           → `clean` + `all`.
- `run-server`        → Run the server binary (uses Valgrind wrapper in `valgrind-debug` mode).
- `clean`             → Remove all build artifacts (`build/`).
- `clean-obj`         → Remove only object files.
- `compile_commands`  → Generate `compile_commands.json` for clangd / language servers.

## Example Commands

### Basic build (release mode, default):  

```bash
make
make all