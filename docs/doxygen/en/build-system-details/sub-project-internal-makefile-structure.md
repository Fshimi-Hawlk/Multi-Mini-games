@page sub_project_internal_makefile_structure Sub-Project Internal Makefile Structure

@ref index "Back to Home"

**Last checked against project structure:** March 16, 2026

This page explains the **internal structure** of the Makefile system used in each sub-project (games, lobby, etc.).

We use a **modular split-Makefile** design to keep things maintainable, readable and easier to extend later (especially for different platforms).

All real Makefile logic lives in the `make/` folder.  
The root `Makefile` in each sub-project is kept very short — it only includes the modular files in a fixed order and declares `.PHONY` targets.

## Directory Layout

```text
sub-project-root/
├── Makefile                  # very short: includes + .PHONY
├── make/
│   ├── 00-config.mk          # compiler, modes, flags, paths, OS detection
│   ├── 10-sources.mk         # finds .c files in src/ and tests/, builds object lists
│   ├── 20-build-rules.mk     # pattern rules (.o from .c, linking, static-lib)
│   ├── 30-targets-main.mk    # main program targets (all, rebuild, run-main, etc.)
│   ├── 40-targets-tests.mk   # test targets (tests, run-tests with logs)
│   ├── 50-tools.mk           # verbosity control, clean, help, stdbuf handling
│   ├── 99-overrides.mk       # optional local overrides (git-ignored)
│   └── platform/             # OS-specific settings (included from 00-config.mk)
│       ├── linux.mk
│       ├── darwin.mk
│       └── mingw.mk
├── makefile.md               # user-facing guide (make help, modes, examples)
├── src/
├── tests/
├── build/                    # generated files
└── ...
```

## Inclusion Order (very important)

The root Makefile includes these files **exactly in this sequence**:

1. `00-config.mk`     - sets compilers, flags, MODE, directories, detects OS, includes platform/*.mk  
2. `10-sources.mk`    - scans src/ and tests/ for .c files, creates object & dependency lists  
3. `20-build-rules.mk` - defines how to compile .o files and link executables/static libs  
4. `30-targets-main.mk` - user targets for the main program (all, rebuild, run-main, run-gdb, static-lib)  
5. `40-targets-tests.mk` - test-related targets (tests, run-tests with timestamped logs)  
6. `50-tools.mk`      - helper stuff: silent/verbose mode, clean-all, help output  
7. `99-overrides.mk`  - last (optional) file for local machine tweaks, never committed

Early files mostly define variables.  
Later files use those variables to create actual rules and targets.

## Why split and number this way?

- Make evaluates files in the order they are included — numbering forces predictable behavior  
- Clear separation of concerns makes it easier to find/edit one part without breaking others  
  - 00 = setup & environment  
  - 10 = source discovery  
  - 20 = core build mechanics  
  - 30/40 = what people actually run (make run-main, make tests, etc.)  
  - 50 = developer convenience tools  
  - 99 = personal overrides without polluting git  
- Platform-specific stuff stays isolated in `platform/` so we can add Windows/macOS/Linux differences cleanly later

## Quick Reference Table

| File                  | Main job                                               | Important things it defines / creates              |
|-----------------------|--------------------------------------------------------|----------------------------------------------------|
| 00-config.mk          | Compiler, flags, modes, paths, OS detection            | CC, CFLAGS, LDFLAGS, MODE, OBJ_DIR, BIN, platform include |
| 10-sources.mk         | Scans for source files, builds lists                   | LIB_SOURCES, LIB_OBJECTS, TEST_BINS, DEPS          |
| 20-build-rules.mk     | Compilation & linking rules                            | %.o pattern, $(BIN), $(STATIC_LIB), test linking   |
| 30-targets-main.mk    | Main program targets                                   | all, rebuild, run-main, run-gdb, static-lib        |
| 40-targets-tests.mk   | Test building & running with logs                      | tests, run-tests                                   |
| 50-tools.mk           | Verbose control, clean, help, stdbuf wrapper           | SILENT_PREFIX, clean, help                         |
| 99-overrides.mk       | Local machine tweaks (git ignored)                     | any override you need                              |
| platform/*.mk         | OS-specific flags & libs                               | BASE_CFLAGS, BASE_LDFLAGS, raylib paths, etc.      |

## Related pages

- @ref sub_project_makefile "User-facing Makefile guide" — how to actually use the targets and modes  

**Created:** February 23, 2026  
**Last updated:** March 16, 2026  
**Author:** [Fshimi Hawlk](https://github.com/Fshimi-Hawlk)

@ref index "Back to Home"