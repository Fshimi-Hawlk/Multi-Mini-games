# ---------------------------------------------------------------
# Makefile - Project build system
#
#     Main targets: see 'make help' for the full list and descriptions
#     Modes:        release, debug, strict-debug, clang-debug, valgrind-debug
#
#     Full documentation ->  makefile.md
#     Quick usage        ->  make help
#
# Author: Fshimi Hawlk <https://github.com/Fshimi-Hawlk>
# ---------------------------------------------------------------

# ---------------------------------------------------------------
# Auto-detect game libraries when built standalone (not via root Makefile).
# Must be set BEFORE 00-config.mk so EXTRA_LDFLAGS is set when
# LDFLAGS += $(EXTRA_LDFLAGS) is evaluated during that include.
# ---------------------------------------------------------------
ifeq ($(strip $(EXTRA_LDFLAGS)),)
    _GAME_LIBS := $(wildcard ../build/lib/lib*.a)
    ifneq ($(strip $(_GAME_LIBS)),)
        $(info [lobby] Auto-detected game libs: $(_GAME_LIBS))
        EXTRA_LDFLAGS := $(_GAME_LIBS)
    else
        $(info [lobby] No game libs in ../build/lib/ — run 'make libs' from project root first.)
    endif
endif

MAKEFILE_DIR := $(dir $(realpath $(lastword $(MAKEFILE_LIST))))
include $(MAKEFILE_DIR)make/00-config.mk
include $(MAKEFILE_DIR)make/10-sources.mk
include $(MAKEFILE_DIR)make/20-build-rules.mk
include $(MAKEFILE_DIR)make/30-targets-main.mk
include $(MAKEFILE_DIR)make/40-targets-tests.mk
include $(MAKEFILE_DIR)make/50-tools.mk

# Optional local overrides (not in git)
-include $(MAKEFILE_DIR)make/99-overrides.mk

.PHONY: all clean rebuild static-lib run-main run-gdb run-tests tests docs doxygen clean-docs help
