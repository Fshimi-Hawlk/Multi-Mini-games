# ╭────────────────────────────────────────────────────────────────────────╮
# │                     firstparty Static Library Makefile                 │
# │                                                                        │
# │  Builds libfirstparty.a containing shared utilities, logger, arena,    │
# │  system settings, widgets and other first-party modules.               │
# │                                                                        │
# │  Main entry points:                                                    │
# │    • make static-lib     ─ build the library (default target)          │
# │    • make clean          ─ remove all build artifacts                  │
# │    • make help           ─ show available targets and options          │
# │                                                                        │
# │  Build modes (via MODE=...):                                           │
# │    release, debug, strict-debug, clang-debug, valgrind-debug           │
# │  Verbose output: VERBOSE=1                                             │
# │                                                                        │
# │  Author:   Fshimi-Hawlk                                                │
# │  Updated:  2026-04-04                                                  │
# ╰────────────────────────────────────────────────────────────────────────╯


# ───────────────────────────────────────────────────────────────
# Configuration
# ───────────────────────────────────────────────────────────────

MODE          ?= release
VERBOSE       ?= 0
LIB_NAME      ?= firstparty

# Verbose control
ifeq ($(VERBOSE),1)
    SILENT_PREFIX :=
else
    SILENT_PREFIX := @
endif

# Directories
ROOT          := .
BUILD_DIR     := ${ROOT}/build
LIB_DIR       := $(BUILD_DIR)/lib
OBJ_DIR       := $(BUILD_DIR)/obj

# Compiler / tools
CC            := gcc
AR            := ar
ARFLAGS       := rcs

# ───────────────────────────────────────────────────────────────
# Mode-specific flags (aligned with root 00-config.mk)
# ───────────────────────────────────────────────────────────────

ifeq ($(MODE),release)
    CFLAGS_MODE := -O2 -DNDEBUG
    LDFLAGS_MODE :=
else ifeq ($(MODE),debug)
    CFLAGS_MODE := \
        -Wall \
        -Wextra \
        -g \
        -O0 \
        -Wno-unused-function \
        -Wno-deprecated-declarations \
        -Wno-macro-redefined \
        -D_STACK_TRACE \
        -D_DEBUG
    LDFLAGS_MODE := -g -rdynamic -O0
else ifeq ($(MODE),strict-debug)
    CFLAGS_MODE := \
        -Werror \
        -Wall \
        -Wextra \
        -pedantic \
        -g \
        -O0 \
        -Wno-unused-function \
        -Wno-deprecated-declarations \
        -Wno-macro-redefined \
        -D_STACK_TRACE \
        -D_DEBUG
    LDFLAGS_MODE := -g -rdynamic -O0 -fno-omit-frame-pointer
else ifeq ($(MODE),clang-debug)
    ifeq ($(shell command -v clang >/dev/null 2>&1; echo $$?),0)
        CC := clang
        CFLAGS_MODE := \
            -Werror \
            -Wall \
            -Wextra \
            -pedantic \
            -g \
            -O0 \
            -Wno-macro-redefined \
            -Wno-newline-eof \
            -Wno-unused-function \
            -Wno-deprecated-declarations \
            -Wno-gnu-zero-variadic-macro-arguments \
            -fno-omit-frame-pointer \
            -fsanitize=address,undefined \
            -D_STACK_TRACE \
            -D_DEBUG
        LDFLAGS_MODE := \
            -g \
            -rdynamic \
            -O0 \
            -fno-omit-frame-pointer \
            -fsanitize=address,undefined
    else
        $(error Clang not detected. Use debug or strict-debug instead.)
    endif
else ifeq ($(MODE),valgrind-debug)
    ifneq ($(shell uname -s),Linux)
        $(error valgrind-debug mode is only supported on Linux)
    endif
    ifeq ($(shell command -v valgrind >/dev/null 2>&1; echo $$?),0)
        CFLAGS_MODE := \
            -Werror \
            -Wall \
            -Wextra \
            -pedantic \
            -g \
            -O0 \
            -Wno-macro-redefined \
            -Wno-unused-function \
            -Wno-deprecated-declarations \
            -D_STACK_TRACE \
            -D_DEBUG
        LDFLAGS_MODE := -g -rdynamic -O0
    else
        $(error Valgrind not detected. Use debug or strict-debug instead.)
    endif
else
    $(error Unknown MODE=$(MODE). Supported: release, debug, strict-debug, clang-debug, valgrind-debug)
endif

# Base flags (always present) + mode flags
BASE_CFLAGS := 		\
	-I$(ROOT)		\
	-I$(ROOT)/utils	\
	-I..  			\
	-I../thirdparty \

CFLAGS 	:= $(BASE_CFLAGS) $(CFLAGS_MODE) $(EXTRA_CFLAGS)

# ───────────────────────────────────────────────────────────────
# Source discovery
# ───────────────────────────────────────────────────────────────

# All .c files under src/ (preserves subdirectories like src/logger/, src/utils/, etc.)
SRCS          := $(shell find src -name '*.c')

# Object files (preserve directory structure under obj/)
OBJS          := $(patsubst src/%.c,$(OBJ_DIR)/%.o,$(SRCS))

# Library output
LIB           := $(LIB_DIR)/lib$(LIB_NAME).a

# ───────────────────────────────────────────────────────────────
# Phony targets
# ───────────────────────────────────────────────────────────────

.PHONY: all static-lib clean help

all: static-lib

# ───────────────────────────────────────────────────────────────
# Directory creation (top-level only)
# ───────────────────────────────────────────────────────────────

$(LIB_DIR) $(OBJ_DIR):
	$(SILENT_PREFIX)mkdir -p $@

# ───────────────────────────────────────────────────────────────
# Pattern rule for object files (with dependency tracking)
# ───────────────────────────────────────────────────────────────

$(OBJ_DIR)/%.o: src/%.c | $(OBJ_DIR)
	$(SILENT_PREFIX)mkdir -p $(dir $@)
	$(SILENT_PREFIX)$(CC) $(CFLAGS) -MMD -MP -c $< -o $@

# ───────────────────────────────────────────────────────────────
# Static library target
# ───────────────────────────────────────────────────────────────

static-lib: $(LIB)

$(LIB): $(OBJS) | $(LIB_DIR)
	$(SILENT_PREFIX)$(AR) $(ARFLAGS) $@ $^

# ───────────────────────────────────────────────────────────────
# Clean
# ───────────────────────────────────────────────────────────────

clean:
	@echo "Cleaning firstparty build artifacts..."
	$(SILENT_PREFIX)rm -rf $(BUILD_DIR)
	@echo "firstparty clean complete."

# ───────────────────────────────────────────────────────────────
# Help
# ───────────────────────────────────────────────────────────────

help:
	@echo "firstparty/Makefile - build libfirstparty.a"
	@echo ""
	@echo "Targets:"
	@echo "  static-lib (default)   Build the static library"
	@echo "  clean                  Remove build artifacts"
	@echo "  help                   Show this message"
	@echo ""
	@echo "Variables (override with make VAR=value):"
	@echo "  MODE=release|debug|strict-debug|clang-debug|valgrind-debug"
	@echo "                         (default: release)"
	@echo "  VERBOSE=1              Show full commands (default: silent)"
	@echo "  EXTRA_CFLAGS=...       Additional compiler flags"
	@echo "  EXTRA_LDFLAGS=...      Additional linker flags"
	@echo ""
	@echo "Output: build/lib/libfirstparty.a"
	@echo "This library is intended to be consumed by the root Makefile."