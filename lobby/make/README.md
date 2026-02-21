# Makefile System Structure

This project uses a **modular, split Makefile** approach for better maintainability, readability and platform extensibility.

All Makefile logic lives inside the `make/` directory.  
The root `Makefile` is kept minimal - it only includes the modular pieces in a strict numbered order.

## Directory Layout
```bash
sub-project-root/
├── Makefile                  # <- very short: just includes + .PHONY
├── make/
│   ├── 00-config.mk          # Compiler, modes, flags, directories, OS detection
│   ├── 10-sources.mk         # Source file discovery (src/, tests/), object & binary lists
│   ├── 20-build-rules.mk     # Pattern rules (.o <- .c, linking main/tests/static-lib)
│   ├── 30-targets-main.mk    # Main binary targets: all, rebuild, run-main, run-gdb, static-lib
│   ├── 40-targets-tests.mk   # Test targets: tests, run-tests (with detailed logging)
│   ├── 50-tools.mk           # Verbosity, dependency tracking, clean, help, stdbuf logic
│   ├── 99-overrides.mk       # Optional, git-ignored local overrides
│   └── platform/             # <- platform-specific configuration (added later)
│       ├── linux.mk
│       ├── darwin.mk
│       └── mingw.mk
├── makefile.md               # User-facing build instructions ("make help", modes, examples)
├── src/
├── tests/
├── build/                    # generated
└── ...
```
## Inclusion Order (important!)

The files are included **in this exact sequence** from the root Makefile:

1. `00-config.mk`     -> defines CC, CFLAGS, LDFLAGS, MODE logic, directories, OS detection, platform include
2. `10-sources.mk`    -> discovers .c files -> generates object lists, dependency lists, BIN/TEST_BINS names
3. `20-build-rules.mk`-> actual compilation & linking rules (the heart of how things get built)
4. `30-targets-main.mk` -> high-level targets for the main program
5. `40-targets-tests.mk` -> high-level targets for unit/integration tests
6. `50-tools.mk`      -> utilities (silent/verbose, clean, help text, stdbuf warning)
7. `99-overrides.mk`  -> last, optional, user-local tweaks (not in version control)

Early files define variables and discovery logic.  
Later files consume those variables to create rules and targets.

## Why this numbering & splitting?

- **Predictable evaluation order** - Make evaluates variables and rules in inclusion order  
- **Separation of concerns**  
  - 00 = configuration & environment  
  - 10 = source enumeration  
  - 20 = low-level build mechanics  
  - 30/40 = high-level user-facing targets  
  - 50 = tooling & developer UX  
  - 99 = escape hatch for local hacks  
- Easy to grep / reason about one concern at a time  
- Platform differences isolated in `platform/*.mk` (included from `00-config.mk`)

## Quick Reference - What lives where

| File                  | Main responsibility                                   | Key variables / targets defined                     |
|-----------------------|-------------------------------------------------------|-----------------------------------------------------|
| 00-config.mk          | Compiler, flags, modes, paths, OS/platform include    | CC, CFLAGS, LDFLAGS, MODE, TOOL, BIN, OBJ_DIR, …   |
| 10-sources.mk         | Finding source files, building object/binary lists    | LIB_SOURCES, LIB_OBJECTS, TEST_BINS, DEPS          |
| 20-build-rules.mk     | Pattern rules (.o, linking, static lib archiving)     | %.o, $(BIN), $(TEST_BIN_DIR)/%, $(STATIC_LIB)      |
| 30-targets-main.mk    | Main program targets                                  | all, rebuild, run-main, run-gdb, static-lib        |
| 40-targets-tests.mk   | Test program targets & test runner with logging       | tests, run-tests                                   |
| 50-tools.mk           | Verbose/silent, dependency tracking, clean, help      | SILENT_PREFIX, clean, help                         |
| 99-overrides.mk       | Local machine-specific overrides (git ignored)        | anything you want to override                      |
| platform/*.mk         | OS-specific flags, libraries, frameworks              | BASE_CFLAGS, BASE_LDFLAGS, RAYLIB_LIB_DIR, …       |

## Related documentation

- **[makefile.md](../makefile.md)**     -> How to **use** the Makefile (targets, modes, examples, logging behavior)  
- This file           -> How the Makefile is **internally structured**

**Last updated: February 21, 2026**  
**Author: [Fshimi Hawlk](https://github.com/Fshimi-Hawlk)**