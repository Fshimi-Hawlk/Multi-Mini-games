@page root_makefile Root Makefile Documentation

# Root Makefile Documentation

This file documents the @b root-level Makefile of the Multi Mini-Games monorepo.

It allows building all merged games' static libraries, copying their API headers to @c firstparty/APIs/, and linking everything into the lobby executable - with lazy/incremental behavior whenever possible.

@section overview Overview

- Builds static libraries (@c lib<gamename>.a) from each module folder (@c tetris/, @c block-blast/, etc.)  
- Copies each module's @c <gamename>API.h -> @c firstparty/APIs/  
- Links all libraries into the lobby executable  
- Supports the same build modes as sub-project Makefiles  
- Output locations:  
  - Libraries: @c build/lib/lib<gamename>.a  
  - Lobby executable: @c build/bin/<MAIN_NAME> (default: @c main)  

@section build_modes Supported Build Modes

Same as sub-projects:

| Mode              | Compiler | Key flags                              | Requirements       |
|-------------------|----------|----------------------------------------|--------------------|
| @c release         | gcc      | @c -O2                                  | -                  |
| @c debug           | gcc      | @c -Wall -Wextra -g -O0                 | -                  |
| @c strict-debug    | gcc      | @c -Werror -Wall -Wextra -pedantic -g   | -                  |
| @c clang-debug     | clang    | + @c -fsanitize=address,undefined       | Clang              |
| @c valgrind-debug  | gcc      | + @c -g -O0                             | Valgrind           |

@note Use @c MODE=... in commands, e.g., @c make MODE=debug bin.

@section main_targets Main Targets

- @c all / @c bin: Default - build libraries + lobby executable (incremental)
- @c libs: Build all game static libraries (lazy)
- @c rebuild: Full clean + build everything
- @c clean: Remove build artifacts
- @c help: Show all targets and modes

@section common_commands Common Commands & Examples

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

@section asset_handling Asset Path Handling

Each module sets its own asset prefix at compile time:  

- Standalone build (inside game folder): @c assets/...  
- Root build: @c <module-name>/assets/... (e.g. @c tetris/assets/...)  

This is done via @c -DASSET_PATH="..." passed through EXTRA_CFLAGS.  

@section api_headers API Headers

When building libraries (@c libs, @c bin, @c rebuild-libs):

- Each module's @c <module-name>API.h (e.g. @c tetrisAPI.h) is copied from @c <module>/include/ to @c firstparty/APIs/
- Copy happens only if the file exists

@section logging_debug Logging & Debugging

- @c run-tests creates timestamped folders in @c logs/tests-<timestamp>/
- In @c valgrind-debug mode: also creates @c logs/valgrind-<timestamp>/
- Use @c VERBOSE=1 to see full commands

@subsection portability Portability / Requirements

- @c clang-debug -> needs @c clang
- @c valgrind-debug -> needs @c valgrind
- @c cmp (used to skip unchanged files) -> almost always present on Unix-like systems

**Created:** March 02, 2026  
**Author:** [Fshimi Hawlk](https://github.com/Fshimi-Hawlk)