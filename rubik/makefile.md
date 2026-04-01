# Makefile Documentation

This file documents the project's main Makefile.  
It is a comprehensive build script supporting multiple compilation modes, automatic dependency tracking, separate test binaries, logging, and runtime tools like Valgrind.

## Overview

The Makefile manages:
- Building the main executable from `src/` (excluding the main entry point)
- Building independent test executables from `tests/`
- Linking shared library objects to both main and tests (avoids multiple-definition errors)
- Multiple build modes: release, basic-debug, strict-debug, clang-debug (sanitizers), valgrind-debug
- Automatic header dependency tracking (`-MMD -MP`) — disable with `NO_DEPENDENCY_TRACKING=1`
- Silent builds by default — enable verbose output with `VERBOSE=1`
- Custom flags via `EXTRA_CFLAGS` / `EXTRA_LDFLAGS`
- Logging for tests and valgrind runs in timestamped folders under `logs/`

Output locations:
- Object files: `build/obj/`
- Main binary: `build/bin/<MAIN_NAME>` (default: `build/bin/main`)
- Test binaries: `build/bin/tests/<test-name>`

## Supported Build Modes

| Mode            | Compiler | Flags / Features                              | Requirements          | Notes                                      |
|-----------------|----------|-----------------------------------------------|-----------------------|--------------------------------------------|
| `release`       | gcc      | `-O2`                                         | None                  | Optimized, no debug info                   |
| `basic-debug`   | gcc      | `-Wall -Wextra -g -O0`                        | None                  | Basic warnings + debug symbols             |
| `strict-debug`  | gcc      | `-Werror -Wall -Wextra -pedantic -g -O0`      | None                  | Strict warnings, no sanitizers             |
| `clang-debug`   | clang    | `-Werror -pedantic -fsanitize=address,undefined` | Clang installed     | Compile-time sanitizers (ASan + UBSan)     |
| `valgrind-debug`| gcc      | `-Wall -Wextra -g -O0`                        | Valgrind installed    | Debug build + runtime Valgrind checks      |

Default: `release`

## Key Targets

- `all`               → Build main executable (default target)
- `tests`             → Build all test executables
- `run-tests`         → Build + run all tests (live stdout + per-test logs in `logs/tests-<timestamp>/`)
- `rebuild`           → `clean` + `all`
- `rebuild-tests`     → `clean` + `tests`
- `run-main`          → Run main binary (uses Valgrind wrapper in `valgrind-debug` mode)
- `run-gdb`           → Run main binary under gdb
- `clean`             → Remove all build artifacts (`build/`)
- `clean-obj`         → Remove only object files
- `compile_commands`  → Generate `compile_commands.json` for clangd / language servers

## Example Commands

### Basic build (release mode, default):  

```bash
make
make all
```

### Debug builds:  

```bash
make MODE=basic-debug
make MODE=strict-debug
```

### Sanitizers / Valgrind:  

```bash
make MODE=clang-debug
make MODE=valgrind-debug run-main
make MODE=valgrind-debug run-tests
```

make MODE=valgrind-debug run-tests

```bash
make run-tests MODE=basic-debug
make run-tests MODE=clang-debug
```

### Custom main file or extras:  

```bash
make MAIN_NAME=app MODE=release
make EXTRA_CFLAGS="-Wshadow -Wconversion" MODE=strict-debug VERBOSE=1
```

### Full rebuild + run/debug:  

```bash
make rebuild run-main
make rebuild-tests
make run-gdb
```

## Logging Behavior
- `run-tests`: Creates `logs/tests-<dd-mm-YYYY-HH-MM-SS>/` with per-test `.log` files (stdout + stderr)  
- `valgrind-debug` mode: `Creates logs/valgrind-<dd-mm-YYYY-HH-MM-SS>/` with Valgrind output + stdout/stderr  
- Logs only include sections (STDOUT/STDERR) if content exists  

## Author
[Fshimi Hawlk](https://github.com/Fshimi-Hawlk)