## @file    Makefile
## @brief   Comprehensive Makefile for managing compilation, testing, and execution 
##          of a C project with multiple build modes, test support, and logging capabilities.
## 
## @details This Makefile handles building the main executable from sources in 'src' 
##          and separate test executables from sources in 'tests'.
##          
##          Sources in 'src' are split into:
##            - Library objects: all *.c files except the main entry point (default: main.c)
##            - Main object:     the entry point file specified by MAIN_NAME (default: main)
##          
##          The library objects are linked into both the main executable and every test binary,
##          while the main entry point is only linked into the primary executable.
##          This separation prevents multiple-definition errors when building tests.
##          
##          Supported compilation modes:
##            - release         : optimized build using GCC
##            - debug           : warnings + debug symbols using GCC
##            - strict-debug    : pedantic checks without sanitizers using GCC
##            - clang-debug     : sanitizers + pedantic checks using Clang (requires clang installed)
##            - valgrind-debug  : debug build using GCC, runs under Valgrind (requires valgrind installed)
##          
##          Object files → build/obj/
##          Main executable → build/bin/<name>
##          Test executables → build/bin/tests/<name>
##          
##          Automatic dependency tracking is ENABLED by default (-MMD -MP).
##          Use NO_DEPENDENCY_TRACKING=1 to disable.
##          
##          Custom flags via EXTRA_CFLAGS / EXTRA_LDFLAGS.
##          Build is silent by default; use VERBOSE=1 to show commands.
##          
##          Logging (in valgrind-debug and run-tests):
##            - Creates timestamped folder: logs/valgrind-<dd-mm-YYYY-HH-MM-SS>/ or logs/tests-<dd-mm-YYYY-HH-MM-SS>/
##            - Captures stdout/stderr + Valgrind diagnostics in .log files
## 
## @section Key targets
##            - all              : build main executable (default)
##            - tests            : build all test executables
##            - run-tests        : build and run all tests, showing stdout live,
##                                 saving full stdout+stderr to timestamped log files
##                                 (format: dd-mm-YYYY-HH-MM-SS) with conditional banners
##            - rebuild          : clean and rebuild main
##            - rebuild-tests    : clean and rebuild tests
##            - run-main         : run the main binary
##            - run-gdb          : debug the main binary with gdb
##            - clean            : clean build folder and remove it
##            - compile_commands : generate compile_commands.json for clangd
## 
## @section Examples — Example Commands
## 
##   Basic build (release mode, default):
##     make
##     make all
## 
##   Debug builds:
##     make MODE=debug
##     make MODE=strict-debug
## 
##   Sanitizers / Valgrind:
##     make MODE=clang-debug
##     make MODE=valgrind-debug run-main
##     make MODE=valgrind-debug run-tests
## 
##   Tests with live output + logs:
##     make run-tests MODE=debug
##     make run-tests MODE=clang-debug
## 
##   Custom main file or extras:
##     make MAIN_NAME=app MODE=release
##     make EXTRA_CFLAGS="-Wshadow -Wconversion" MODE=strict-debug VERBOSE=1
## 
##   Full rebuild + run/debug:
##     make rebuild run-main
##     make rebuild-tests
##     make run-gdb
## 
## */ @author  Fshimi Hawlk <https://github.com/Fshimi-Hawlk>

# Compiler and flags
# Base flags (always present)
BASE_CFLAGS := \
	-Wno-newline-eof \
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

# compile_commands.json for clangd
compile_commands:
	@echo '[' > compile_commands.json
	@for src in $(LIB_SOURCES) $(MAIN_SOURCE) $(TEST_SOURCES); do \
		obj=$$(echo $$src | sed 's|$(SRC_DIR)|$(OBJ_DIR)|' | sed 's|$(TEST_DIR)|$(OBJ_DIR)/tests|' | sed 's/\.c/\.o/'); \
		echo '{ "directory": "$(PWD)", "command": "$(CC) $(CFLAGS) -c '"$$src"' -o '"$$obj"'", "file": "'$$src'" },'; \
	done >> compile_commands.json
	@sed -i '$$ s/,$$/]/' compile_commands.json

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

# Run all test executables:
# - Normal output (stdout) shown live in terminal
# - Both stdout and stderr saved to log file
# - In the log file: only show banners and sections if there is actual content
run-tests: tests
	$(eval TIMESTAMP := $(shell date "+%d-%m-%Y-%H-%M-%S"))
	$(eval LOG_DIR := logs/tests-$(TIMESTAMP))
	@mkdir -p "$(LOG_DIR)"
	@echo "Running all tests (stdout shown below, full stdout+stderr saved in $(LOG_DIR))..."
	@echo "====================================================================="
	@all_passed=1; \
	for test_bin in $(TEST_BINS); do \
		if [ -f "$$test_bin" ]; then \
			test_name=$$(basename "$$test_bin"); \
			if [ "$(MODE)" = "valgrind-debug" ]; then \
				log_file="$(LOG_DIR)/valgrind-$$test_name.log"; \
			else \
				log_file="$(LOG_DIR)/$$test_name.log"; \
			fi;
			echo ">>> Running $$test_name..."; \
			echo ""; \
			echo "--- Output ---------------------------------------------------------"; \
			\
			# Capture stdout and stderr separately \
			stdout_tmp=$$(mktemp); \
			stderr_tmp=$$(mktemp); \
			\
			$(TOOL) ./$$test_bin >"$$stdout_tmp" 2>"$$stderr_tmp"; \
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
			# Show stdout live in terminal \
			cat "$$stdout_tmp"; \
			\
			# Clean up temp files \
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
	fi

help:
	@echo "Usage: make [TARGET] [OPTIONS]"
	@echo ""
	@echo "TARGETS:"
	@echo "    help                 Print this help message"
	@echo "    all                  Build the executable (default)"
	@echo "    rebuild              Force the recompilation of the entire codebase"
	@echo "    rebuild-tests        Force the recompilation of the test executables"
	@echo "    run-main             Run the app"
	@echo "    run-gdb              Run the app in gdb"
	@echo "    run-tests            Build and run all tests (fails on first error)"
	@echo "    clean                Remove all build artifacts and build folder"
	@echo "    compile_commands     Generate compile_commands.json for clangd"
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
	@echo "    VERBOSE=1                   Show all compilation commands (default: silent)"
	@echo "    NO_DEPENDENCY_TRACKING=1    Disable automatic dependency tracking (.d files)"
	@echo "    EXTRA_CFLAGS=\"<str>\"        Add custom compiler flags"
	@echo "    EXTRA_LDFLAGS=\"<str>\"       Add custom linker flags"

.PHONY: all clean rebuild run-main run-gdb run-tests tests compile_commands help