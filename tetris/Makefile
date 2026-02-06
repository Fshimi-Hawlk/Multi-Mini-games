# ───────────────────────────────────────────────────────────────
# Makefile ─ Project build system
#
#     Main targets: see 'make help' for the full list and descriptions
#     Modes:        release, debug, strict-debug, clang-debug, valgrind-debug
#
#     Full documentation →  makefile.md
#     Quick usage        →  make help
#
# Author: Fshimi Hawlk <https://github.com/Fshimi-Hawlk>
# ───────────────────────────────────────────────────────────────

# Compiler and flags
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
		-Wno-newline-eof \
		-Wno-unused-function \
		-Wno-deprecated-declarations \
		-Wno-gnu-zero-variadic-macro-arguments \
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
		-g \
		-O0 \
		-Wno-unused-function \
		-Wno-deprecated-declarations \
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
		-Wno-newline-eof \
		-Wno-unused-function \
		-Wno-deprecated-declarations \
		-Wno-gnu-zero-variadic-macro-arguments \
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

BUILD_DIR := build
OBJ_DIR := $(BUILD_DIR)/obj
BIN_DIR := $(BUILD_DIR)/bin
TEST_BIN_DIR := $(BUILD_DIR)/bin/tests

BIN := $(BIN_DIR)/$(MAIN_NAME)

# Library/shared sources/objects (recursive, excluding main.c)
LIB_SOURCES := $(shell find $(SRC_DIR) -name '*.c' ! -name '$(MAIN_NAME).c')
LIB_OBJECTS := $(LIB_SOURCES:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

# Main source/object
MAIN_SOURCE := $(SRC_DIR)/$(MAIN_NAME).c
MAIN_OBJECT := $(OBJ_DIR)/$(MAIN_NAME).o

# Test sources/objects/bins (recursive)
TEST_SOURCES := $(shell find $(TEST_DIR) -name '*.c')
TEST_OBJECTS := $(TEST_SOURCES:$(TEST_DIR)/%.c=$(OBJ_DIR)/tests/%.o)
TEST_BINS := $(TEST_SOURCES:$(TEST_DIR)/%.c=$(TEST_BIN_DIR)/%)

# Output buffering helper (improves crash visibility)
STDBUF :=
ifneq ($(shell command -v stdbuf 2>/dev/null),)
    STDBUF := stdbuf --output=L --error=L
else
    # We check again only when run-tests is actually called
    STDBUF_MISSING_MSG := stdbuf not found (from GNU coreutils). Last printf lines may be missing on crash/abort.\nPlease add 'fflush(stdout);' after important prints or install coreutils.
endif

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
	# All dependency files
	DEPS := $(LIB_OBJECTS:.o=.d) $(MAIN_OBJECT:.o=.d) $(TEST_OBJECTS:.o=.d)
endif

# Include dependency files only when tracking is enabled
ifneq ($(NO_DEPENDENCY_TRACKING),1)
	-include $(DEPS)
endif

# Prevent make from deleting intermediate object files
.SECONDARY: $(LIB_OBJECTS) $(MAIN_OBJECT) $(TEST_OBJECTS)

# Rules
all: $(BIN)

tests: $(TEST_BINS)
	@if [ -z "$(TEST_BINS)" ]; then \
		echo "No tests found — nothing to build."; \
	else \
		echo "Test binaries built: $(notdir $(TEST_BINS))"; \
	fi

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
	$(SILENT_PREFIX)rm -rf $(BUILD_DIR)

rebuild: clean all

rebuild-tests: clean tests

run-main:
	@if [ -f $(BIN) ]; then \
		if [ "$(MODE)" = "valgrind-debug" ]; then \
			$(eval TIMESTAMP := $(shell date "+%d-%m-%Y-%H-%M-%S")) \
			$(eval LOG_DIR := logs/valgrind-$(TIMESTAMP)) \
			mkdir -p "$(LOG_DIR)" && \
			log_file="$(LOG_DIR)/main-run-valgrind.log" && \
			echo "Running main under Valgrind... (output saved to $$log_file)" && \
			$(TOOL) $(BIN) > "$$log_file" 2>&1 && \
			echo "Valgrind finished. Log: $$log_file" || \
			(echo "Valgrind failed. See $$log_file" && exit 1); \
		else \
			$(TOOL) $(BIN); \
		fi; \
	else \
		echo "No executable"; \
	fi

run-gdb:
	@if [ -f $(BIN) ]; then gdb $(BIN); else echo "No executable"; fi

run-tests: tests
	@if [ -z "$(TEST_BINS)" ]; then \
		:; \
	else \
		$(eval TIMESTAMP := $(shell date "+%d-%m-%Y-%H-%M-%S")) \
		$(eval LOG_DIR := logs/tests-$(TIMESTAMP)) \
		mkdir -p "$(LOG_DIR)" && \
		if [ -n "$(STDBUF_MISSING_MSG)" ]; then \
			printf "\nWarning: %s\n\n" "$(STDBUF_MISSING_MSG)"; \
		fi && \
		echo "Running all tests (see stdout+stderr in $(LOG_DIR))..." && \
		echo "=====================================================================" && \
		all_passed=1; \
		for test_bin in $(TEST_BINS); do \
			if [ -f "$$test_bin" ]; then \
				test_name=$$(basename "$$test_bin"); \
				if [ "$(MODE)" = "valgrind-debug" ]; then \
					log_file="$(LOG_DIR)/valgrind-$$test_name.log"; \
				else \
					log_file="$(LOG_DIR)/$$test_name.log"; \
				fi; \
				echo ">>> Running $$test_name..."; \
				echo ""; \
				echo "--- Output ---------------------------------------------------------"; \
				\
				stdout_tmp=$$(mktemp); \
				stderr_tmp=$$(mktemp); \
				\
				$(STDBUF) $(TOOL) ./$$test_bin >"$$stdout_tmp" 2>"$$stderr_tmp"; \
				test_status=$$?; \
				\
				{ \
					stdout_content=$$(cat "$$stdout_tmp"); \
					stderr_content=$$(cat "$$stderr_tmp"); \
					\
					if [ -n "$$stdout_content" ]; then \
						echo "==================== STDOUT ===================="; \
						echo "$$stdout_content"; \
					fi; \
					\
					if [ -n "$$stderr_content" ]; then \
						echo ""; \
						echo "==================== STDERR ===================="; \
						echo "$$stderr_content"; \
						echo ""; \
					fi; \
					\
					if [ -n "$$stdout_content" ] || [ -n "$$stderr_content" ]; then \
						echo "================================================"; \
					fi; \
				} > "$$log_file"; \
				\
				cat "$$stdout_tmp"; \
				\
				rm -f "$$stdout_tmp" "$$stderr_tmp"; \
				echo "--------------------------------------------------------------------"; \
				echo ""; \
				if [ $$test_status -eq 0 ]; then \
					echo ">>> $$test_name: PASSED"; \
				else \
					echo ">>> $$test_name: FAILED (details in $$log_file)"; \
					all_passed=0; \
				fi; \
				echo "--------------------------------------------------------------------"; \
				echo ""; \
			else \
				echo "Test binary $$test_bin not found"; \
				all_passed=0; \
			fi; \
		done; \
		echo "====================================================================="; \
		if [ $$all_passed -eq 1 ]; then \
			echo "All tests passed."; \
		else \
			echo "Some tests failed."; \
			exit 1; \
		fi; \
	fi

help:
	@echo "Usage: make [OPTIONS] [TARGET]"
	@echo ""
	@echo "TARGETS:"
	@echo "    help                 Print this help message"
	@echo "    all                  Build the main executable (default)"
	@echo "    tests                Build all test executables (no execution)"
	@echo "    rebuild              Clean and rebuild the main executable"
	@echo "    rebuild-tests        Clean and rebuild test executables"
	@echo "    run-main             Run the main binary (uses Valgrind in valgrind-debug mode)"
	@echo "    run-gdb              Debug the main binary with gdb"
	@echo "    run-tests            Build and run all tests, reporting failures at the end"
	@echo "    clean                Remove all build artifacts and build folder"
	@echo ""
	@echo "OPTIONS:"
	@echo "    MODE=<str>                  Compilation type: release, debug, strict-debug, clang-debug, valgrind-debug"
	@echo "                                Default: release"
	@echo "                                Note: clang-debug requires Clang installed system-wide."
	@echo "                                Note: valgrind-debug requires Valgrind installed system-wide; runs binaries under Valgrind with leak checks."
	@echo "                                Logging in valgrind-debug and run-tests:"
	@echo "                                  - Creates timestamped folder: logs/valgrind-<dd-mm-YYYY-HH-MM-SS>/ or logs/tests-<dd-mm-YYYY-HH-MM-SS>/"
	@echo "                                  - run-main: main-run-valgrind.log"
	@echo "                                  - run-tests: per-test .log files (valgrind- prefixed when in valgrind-debug)"
	@echo "    MAIN_NAME=<str>             Set custom main entry point file (without .c; default: main)"
	@echo "    VERBOSE=1                   Show all compilation commands (default: silent)"
	@echo "    NO_DEPENDENCY_TRACKING=1    Disable automatic dependency tracking (.d files)"
	@echo "    EXTRA_CFLAGS=\"<str>\"        Add custom compiler flags"
	@echo "    EXTRA_LDFLAGS=\"<str>\"       Add custom linker flags"
	@echo ""
	@echo "Portability Notes:"
	@echo "    run-tests uses stdbuf (from GNU coreutils) if available for reliable output on crashes;"
	@echo "    warns if missing and suggests alternatives (e.g., fflush in test code)."

.PHONY: all clean rebuild run-main run-gdb run-tests tests compile_commands help