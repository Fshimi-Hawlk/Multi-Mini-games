# ───────────────────────────────────────────────────────────────
# OS detection (runs on the machine where "make" is invoked)
# ───────────────────────────────────────────────────────────────
UNAME_S := $(shell uname -s)

ifeq ($(findstring Darwin,$(UNAME_S)),Darwin)
    OS := darwin
else ifeq ($(findstring Linux,$(UNAME_S)),Linux)
    OS := linux
else ifeq ($(findstring MINGW,$(UNAME_S)),MINGW)
    OS := mingw
else ifeq ($(findstring MSYS,$(UNAME_S)),MSYS)
    OS := mingw
else
    $(error Unsupported OS: $(UNAME_S). Supported: Linux, macOS (Darwin), Windows (MinGW/MSYS))
endif

# Platform-specific configuration
include $(MAKEFILE_DIR)make/platform/$(OS).mk

# Executable extension (used by BIN and TEST_BINS)
ifeq ($(OS),mingw)
    EXE_EXT := .exe
else
    EXE_EXT :=
endif

# Compiler and flags

# Modes
MODE ?= release
ifeq ($(MODE),release)
	CC := gcc
	CFLAGS := \
		-O2
	LDFLAGS := \
		-O2
	TOOL := 
else ifeq ($(MODE),debug)
	CC := gcc
	CFLAGS := \
		-Wall \
		-Wextra \
		-g \
		-O0 \
		-Wno-unused-function \
		-Wno-deprecated-declarations \
		-Wno-macro-redefined \
		-D_STACK_TRACE \
		-D_DEBUG
	LDFLAGS := \
		-g \
		-rdynamic \
		-O0
	TOOL := 
else ifeq ($(MODE),strict-debug)
	CC := gcc
	CFLAGS := \
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
	LDFLAGS := \
		-g \
		-rdynamic \
		-O0 \
		-fno-omit-frame-pointer
	TOOL := 
else ifeq ($(MODE),clang-debug)
	ifeq ($(shell command -v clang >/dev/null 2>&1; echo $$?),0)
		CC := clang
		CFLAGS := \
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

		LDFLAGS := \
		-g \
		-rdynamic \
		-O0 \
		-fno-omit-frame-pointer \
		-fsanitize=address,undefined
		TOOL := 
	else
		$(info Clang not detected. Use debug or strict-debug instead.)
		ifeq ($(shell command -v valgrind >/dev/null 2>&1; echo $$?),0)
			$(info Valgrind detected - try MODE=valgrind-debug for runtime checks.)
		endif
		$(error Clang required for clang-debug mode)
	endif
else ifeq ($(MODE),valgrind-debug)
	ifneq ($(OS),linux)
        $(error valgrind-debug mode is only supported on Linux (native Valgrind unavailable on $(OS)))
    endif
	ifeq ($(shell command -v valgrind >/dev/null 2>&1; echo $$?),0)
		CC := gcc
		CFLAGS := \
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

		LDFLAGS := \
		-g \
		-rdynamic \
		-O0
		TOOL := valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes
	else
		$(info Valgrind not detected. Use debug or strict-debug instead.)
		ifeq ($(shell command -v clang >/dev/null 2>&1; echo $$?),0)
			$(info Clang detected - try MODE=clang-debug for compile-time sanitizers.)
		endif
		$(error Valgrind required for valgrind-debug mode)
	endif
else
	$(error Unknown MODE=$(MODE). Use release, debug, strict-debug, clang-debug, valgrind-debug)
endif

# Combine with base
CFLAGS += $(BASE_CFLAGS)
LDFLAGS += $(BASE_LDFLAGS)

# Allow extras from command line
CFLAGS += $(EXTRA_CFLAGS)
LDFLAGS += $(EXTRA_LDFLAGS)

MAIN_NAME ?= main

SRC_DIR := src
TEST_DIR := tests

# Dirs
BUILD_DIR := build
OBJ_DIR := $(BUILD_DIR)/obj
LIB_DIR := $(BUILD_DIR)/lib
BIN_DIR := $(BUILD_DIR)/bin
TEST_BIN_DIR := $(BUILD_DIR)/bin/tests

STATIC_LIB  ?= $(LIB_DIR)/lib$(LIB_NAME).a
BIN := $(BIN_DIR)/$(MAIN_NAME)$(EXE_EXT)