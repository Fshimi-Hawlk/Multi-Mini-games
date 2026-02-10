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
			$(info Valgrind detected — try MODE=valgrind-debug for runtime checks.)
		endif
		$(error Clang required for clang-debug mode)
	endif
else ifeq ($(MODE),valgrind-debug)
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
			$(info Clang detected — try MODE=clang-debug for compile-time sanitizers.)
		endif
		$(error Valgrind required for valgrind-debug mode)
	endif
else
	$(error Unknown MODE=$(MODE). Use release, debug, strict-debug, clang-debug, valgrind-debug)
endif

# Base flags (always present)
BASE_CFLAGS := \
	-Iinclude \
	-I../thirdparty \
	-I../firstparty \

# Linker base
BASE_LDFLAGS := \
	-L../thirdparty/libs/raylib-5.5_linux_amd64 \
	-l:libraylib.a \
	-lm

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
BIN_DIR := $(BUILD_DIR)/bin
TEST_BIN_DIR := $(BUILD_DIR)/bin/tests

BIN := $(BIN_DIR)/$(MAIN_NAME)