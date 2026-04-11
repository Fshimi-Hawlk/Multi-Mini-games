## @file    Makefile
## @brief   Makefile for the Bowling mini-game
## 
## @details Supported compilation modes:
##            - release         : optimized build using GCC
##            - basic-debug     : warnings + debug symbols using GCC
##            - strict-debug    : pedantic checks without sanitizers using GCC
##            - clang-debug     : sanitizers + pedantic checks using Clang
##            - valgrind-debug  : debug build using GCC, runs under Valgrind
##            - wsl-release     : optimized build for WSL on Windows
## 
## @section Key targets
##            - all              : build main executable (default)
##            - static-lib       : build static library libbowling.a
##            - rebuild          : clean and rebuild main
##            - run-main         : run the main binary
##            - run-gdb          : debug the main binary with gdb

# ============================================
# Platform Detection (WSL, Linux, Darwin, MinGW)
# ============================================

# Detect operating system
ifeq ($(OS),Windows_NT)
    DETECTED_OS := Windows
    # Check if running in MSYS/MinGW
    ifneq ($(MSYSTEM),)
        DETECTED_OS := MinGW
    endif
else
    # Check if running in WSL
    WSL_CHECK := $(shell if [ -f /proc/version ]; then grep -qi microsoft /proc/version && echo "WSL" || echo "Linux"; else echo "Linux"; fi)
    DETECTED_OS := $(WSL_CHECK)
    ifeq ($(WSL_CHECK),WSL)
        # Check WSL version (WSL2 has different display handling)
        WSL_VERSION := $(shell if [ -f /proc/version ]; then grep -qi "microsoft" /proc/version | head -c 8; fi)
    endif
endif

# Include platform-specific configuration
PLATFORM_MK := make/platform/$(DETECTED_OS).mk
ifneq ($(wildcard $(PLATFORM_MK)),)
    include $(PLATFORM_MK)
else
    # Fallback to linux if platform not found
    include make/platform/linux.mk
endif

# Additional WSL-specific settings
ifeq ($(DETECTED_OS),WSL)
    # WSL can use Linux raylib but needs Windows display
    EXTRA_CFLAGS += -DPLATFORM_WSL
    # Try to detect if we should use Windows display
    ifneq ($(WAYLAND_DISPLAY),)
        EXTRA_LDFLAGS += -lwayland-client
    endif
endif

# Modes
MODE ?= release
ifeq ($(MODE),release)
	CC := gcc
	CFLAGS := \
		-O3 \
		-ffast-math \
		-march=native
	LDFLAGS := \
		-O3
	TOOL := 
else ifeq ($(MODE),wsl-release)
	CC := gcc
	CFLAGS := \
		-O3 \
		-ffast-math \
		-march=native \
		-fopenmp
	LDFLAGS := \
		-O3 \
		-pthread
	TOOL := 
else ifeq ($(MODE),debug)
# Alias for basic-debug for compatibility with root Makefile
else ifeq ($(MODE),basic-debug)
	CC := gcc
	CFLAGS := \
		-Wall \
		-Wextra \
		-g \
		-O0 \
		-Wno-newline-eof \
		-Wno-unused-function \
		-Wno-deprecated-declarations \
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
		-fno-omit-frame-pointer \
		-Wno-newline-eof \
		-Wno-unused-function \
		-Wno-deprecated-declarations \
		-Wno-gnu-zero-variadic-macro-arguments \
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
		-fno-omit-frame-pointer \
		-fsanitize=address,undefined \
		-Wno-newline-eof \
		-Wno-unused-function \
		-Wno-deprecated-declarations \
		-Wno-gnu-zero-variadic-macro-arguments \
		-Wno-newline-eof \
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
		$(info Clang not detected. Use basic-debug or strict-debug instead.)
		ifeq ($(shell command -v valgrind >/dev/null 2>&1; echo $$?),0)
			$(info Valgrind detected — try MODE=valgrind-debug for runtime checks.)
		endif
		$(error Clang required for clang-debug mode)
	endif
else ifeq ($(MODE),valgrind-debug)
	ifeq ($(shell command -v valgrind >/dev/null 2>&1; echo $$?),0)
		CC := gcc
		CFLAGS := \
		-Wall \
		-Wextra \
		-g \
		-O0 \
		-Wno-newline-eof \
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
		$(info Valgrind not detected. Use basic-debug or strict-debug instead.)
		ifeq ($(shell command -v clang >/dev/null 2>&1; echo $$?),0)
			$(info Clang detected — try MODE=clang-debug for compile-time sanitizers.)
		endif
		$(error Valgrind required for valgrind-debug mode)
	endif
else
	$(error Unknown MODE=$(MODE). Use release, wsl-release, basic-debug, strict-debug, clang-debug, valgrind-debug)
endif

# Combine with base
CFLAGS += $(BASE_CFLAGS)
LDFLAGS += $(BASE_LDFLAGS)

# Allow extras from command line
CFLAGS += $(EXTRA_CFLAGS)
LDFLAGS += $(EXTRA_LDFLAGS)

MAIN_NAME ?= main

# Dirs
SRC_DIR := src
TEST_DIR := tests

OBJ_DIR := build/obj
BIN_DIR := build/bin
LIB_DIR := build/lib
TEST_BIN_DIR := build/bin/tests

BIN := $(BIN_DIR)/$(MAIN_NAME)

# Library/shared sources/objects (recursive, excluding main.c)
LIB_SOURCES := $(shell find $(SRC_DIR) -name '*.c' ! -name '$(MAIN_NAME).c')
LIB_OBJECTS := $(LIB_SOURCES:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

# Main source/object
MAIN_SOURCE := $(SRC_DIR)/$(MAIN_NAME).c
MAIN_OBJECT := $(OBJ_DIR)/$(MAIN_NAME).o

# Test sources/objects/bins (recursive)
TEST_SOURCES := $(shell find $(TEST_DIR) -name '*.c' 2>/dev/null)
TEST_OBJECTS := $(TEST_SOURCES:$(TEST_DIR)/%.c=$(OBJ_DIR)/tests/%.o)
TEST_BINS := $(TEST_SOURCES:$(TEST_DIR)/%.c=$(TEST_BIN_DIR)/%)

# All deps
DEPS := $(LIB_OBJECTS:.o=.d) $(MAIN_OBJECT:.o=.d) $(TEST_OBJECTS:.o=.d)

# Verbose mode (default silent)
ifeq ($(VERBOSE),1)
	SILENT_PREFIX :=
else
	SILENT_PREFIX := @
endif

# Automatic dependency tracking control (enabled by default)
ifeq ($(NO_DEPENDENCY_TRACKING),1)
	DEP_FLAGS :=
	DEPS :=
else
	DEP_FLAGS := -MMD -MP
	DEPS := $(LIB_OBJECTS:.o=.d) $(MAIN_OBJECT:.o=.d) $(TEST_OBJECTS:.o=.d)
endif

# Include dependency files only when tracking is enabled
ifneq ($(NO_DEPENDENCY_TRACKING),1)
	-include $(DEPS)
endif

# Prevent make from deleting intermediate object files
.SECONDARY: $(LIB_OBJECTS) $(MAIN_OBJECT) $(TEST_OBJECTS)

# Rules
.PHONY: all tests static-lib clean clean-obj rebuild rebuild-tests copy-assets run-main run-gdb run-wsl help

all: $(BIN)

tests: $(TEST_BINS)

static-lib: $(LIB_OBJECTS)
	$(SILENT_PREFIX)mkdir -p $(LIB_DIR)
	$(SILENT_PREFIX)ar rcs $(LIB_DIR)/libbowling.a $^

$(BIN): $(LIB_OBJECTS) $(MAIN_OBJECT)
	$(SILENT_PREFIX)mkdir -p $(@D)
	$(SILENT_PREFIX)$(CC) $^ $(LDFLAGS) -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(SILENT_PREFIX)mkdir -p $(@D)
	$(SILENT_PREFIX)$(CC) $(CFLAGS) $(DEP_FLAGS) -c $< -o $@

$(TEST_BIN_DIR)/% : $(OBJ_DIR)/tests/%.o $(LIB_OBJECTS)
	$(SILENT_PREFIX)mkdir -p $(@D)
	$(SILENT_PREFIX)$(CC) $^ $(LDFLAGS) -o $@

$(OBJ_DIR)/tests/%.o: $(TEST_DIR)/%.c
	$(SILENT_PREFIX)mkdir -p $(@D)
	$(SILENT_PREFIX)$(CC) $(CFLAGS) $(DEP_FLAGS) -c $< -o $@

clean:
	$(SILENT_PREFIX)rm -rf $(OBJ_DIR) $(BIN_DIR)/* $(LIB_DIR)

clean-obj:
	$(SILENT_PREFIX)rm -rf $(OBJ_DIR)/*

rebuild: clean all

rebuild-tests: clean tests

copy-assets:
	$(SILENT_PREFIX)mkdir -p $(BIN_DIR)/assets
	$(SILENT_PREFIX)cp -r assets/* $(BIN_DIR)/assets/ 2>/dev/null || true

run-main:
	@if [ -f $(BIN) ]; then \
		if [ "$(MODE)" = "valgrind-debug" ]; then \
			$(eval TIMESTAMP := $(shell date "+%d-%m-%Y-%H-%M-%S")) \
			$(eval LOG_DIR := logs/valgrind-$(TIMESTAMP)) \
			mkdir -p "$(LOG_DIR)" && \
			log_file="$(LOG_DIR)/main-run-valgrind.log" && \
			echo "Running main under Valgrind... (output saved to $log_file)" && \
			$(TOOL) ./$(BIN) > "$log_file" 2>&1 && \
			echo "Valgrind finished. Log: $log_file" || \
			(echo "Valgrind failed. See $log_file" && exit 1); \
		else \
			$(TOOL) ./$(BIN); \
		fi; \
	else \
		echo "No executable"; \
	fi

run-wsl:
	@if [ -f $(BIN) ]; then \
		if [ "$(DETECTED_OS)" = "WSL" ]; then \
			echo "Running on WSL - Make sure Windows display is configured:"; \
			echo "  - For WSL1: export DISPLAY=localhost:0; for WSL2: export DISPLAY=:0;"; \
			$(TOOL) $(BIN); \
		else \
			echo "Not running on WSL. Use 'make run-main' instead."; \
		fi; \
	else \
		echo "No executable"; \
	fi

run-gdb:
	@if [ -f $(BIN) ]; then gdb $(BIN); else echo "No executable"; fi

help:
	@echo "Usage: make [TARGET] [OPTIONS]"
	@echo ""
	@echo "TARGETS:"
	@echo "    help                 Print this help message"
	@echo "    all                  Build the executable (default)"
	@echo "    static-lib           Build static library libbowling.a"
	@echo "    tests                Build all test executables"
	@echo "    rebuild              Force the recompilation of the entire codebase"
	@echo "    rebuild-tests        Force the recompilation of the test executables"
	@echo "    run-main             Run the app"
	@echo "    run-gdb              Run the app in gdb"
	@echo "    run-wsl              Run the app on WSL (with display config help)"
	@echo "    clean                Remove all build artifacts"
	@echo "    clean-obj            Remove object files"
	@echo ""
	@echo "OPTIONS:"
	@echo "    MODE=<str>           Compilation type: release, wsl-release, basic-debug, strict-debug, clang-debug, valgrind-debug"
	@echo "                        Default: release"
	@echo "    VERBOSE=1            Show all compilation commands (default: silent)"
	@echo "    NO_DEPENDENCY_TRACKING=1    Disable automatic dependency tracking (.d files)"
	@echo "    EXTRA_CFLAGS=\"<str>\"        Add custom compiler flags"
	@echo "    EXTRA_LDFLAGS=\"<str>\"       Add custom linker flags"
	@echo ""
	@echo "PLATFORM:"
	@echo "    The Makefile automatically detects: Linux, WSL, Darwin (macOS), MinGW (Windows)"
	@echo "    Use 'wsl-release' mode for optimized build on WSL"
	@echo ""
	@echo "WSL Setup:"
	@echo "    Before running on WSL, configure display:"
	@echo "      - WSL1: export DISPLAY=localhost:0"
	@echo "      - WSL2: export DISPLAY=:0"
	@echo "    Or use VcXsrv/Xming on Windows"
