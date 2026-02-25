# ───────────────────────────────────────────────────────────────
# Lobby Makefile
# Build: make [MODE=release|debug|strict-debug|clang-debug|valgrind-debug]
# ───────────────────────────────────────────────────────────────

MODE ?= debug
VERBOSE ?= 0
MAIN_NAME := lobby

SILENT := $(if $(filter 1,$(VERBOSE)),,@)

.PHONY: all clean rebuild static-lib run-main run-gdb run-tests tests docs doxygen clean-docs help
