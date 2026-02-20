# ───────────────────────────────────────────────────────────────
# Makefile ─ Project build system
#
#     Main targets: see 'make help' for the full list and descriptions
#     Modes:        release, debug, strict-debug, clang-debug, valgrind-debug
#
#     Full documentation ->  makefile.md
#     Quick usage        ->  make help
#
# Author: Fshimi Hawlk <https://github.com/Fshimi-Hawlk>
# ───────────────────────────────────────────────────────────────

MAKEFILE_DIR := $(dir $(realpath $(lastword $(MAKEFILE_LIST))))
include $(MAKEFILE_DIR)make/00-config.mk
include $(MAKEFILE_DIR)make/10-sources.mk
include $(MAKEFILE_DIR)make/20-build-rules.mk
include $(MAKEFILE_DIR)make/30-targets-main.mk
include $(MAKEFILE_DIR)make/40-targets-tests.mk
include $(MAKEFILE_DIR)make/50-tools.mk

# Optional local overrides (not in git)
-include $(MAKEFILE_DIR)make/99-overrides.mk

.PHONY: all clean rebuild static-lib run-main run-gdb run-tests tests help