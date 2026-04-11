# ==============================================================================
# Top-Level Makefile - Project build system
#
#     Main targets: see 'make help' for the full list and descriptions
#     Modes:        release, debug, strict-debug, clang-debug, valgrind-debug
#
#     Quick usage:  make help
#
# Author: Fshimi Hawlk <https://github.com/Fshimi-Hawlk>
# ==============================================================================

# External libraries
RAYLIB_VERSION := 5.5
UNAME_S := $(shell uname -s)

ifeq ($(findstring MINGW,$(UNAME_S)),MINGW)
    EXE_EXT := .exe
endif

# ==============================================================================
# Configuration
# ==============================================================================

MODE ?= release
VERBOSE ?= 0

# Verbose mode (default silent)
ifeq ($(VERBOSE),1)
	SILENT_PREFIX :=
else
	SILENT_PREFIX := @
endif

# Suppress subdirectory messages from recursive make
MAKEFLAGS += --no-print-directory

# Directories to exclude when discovering modules
EXCLUDED_DIRS := \
    assets       \
    build        \
    docs         \
    games        \
    logs         \
    thirdparty   \
    tui-ver

# Detect modules (normalize by removing trailing /)
ROOT_MODULES := $(patsubst %/,%,$(wildcard */))
ROOT_MODULES := $(filter-out $(EXCLUDED_DIRS), $(ROOT_MODULES))

GAMES_MODULES := $(patsubst %/,%,$(wildcard games/*/))

MODULES := $(ROOT_MODULES) $(GAMES_MODULES)

FIRSTPARTY_API_DIR := firstparty/APIs

# Root-level directories
BUILD_DIR := build
LIB_DIR   := $(BUILD_DIR)/lib
BIN_DIR   := $(BUILD_DIR)/bin

# Computed lib names (flattened lowercase, no - or _)
# Uses notdir to handle paths like games/tetris → tetris
define compute-lib-name
$(shell echo '$(notdir $(1))' | tr '[:upper:]' '[:lower:]' | tr -d '_-')
endef

define compute-api-name
$(shell echo '$(1)API.h')
endef

# Absolute lib paths (used for dependency tracking)
LIBS := $(foreach mod,$(MODULES),$(LIB_DIR)/lib$(call compute-lib-name,$(mod)).a)

# Lib paths for linking (relative from lobby/ -> ../../build/lib/...)
LIBS_REL := $(foreach lib,$(LIBS),../$(lib))

# ==============================================================================
# Main Targets
# ==============================================================================

all: client server

# Ensure directories exist
$(BIN_DIR) $(LIB_DIR):
	$(SILENT_PREFIX)mkdir -p $@

# Lazy build of module libraries (only rebuild if sources changed)
libs: $(LIBS)

# Generic rule to build a static library for any module
$(LIB_DIR)/lib%.a:
	$(eval MOD_DIR := $(strip $(foreach m,$(MODULES),$(if $(filter $(call compute-lib-name,$m),$*),$m))))
	
	$(if $(MOD_DIR),,$(error Could not find module directory for lib$*.a - check MODULES and compute-lib-name))

	$(eval LIB_NAME := $(call compute-lib-name,$(MOD_DIR)))
	$(eval API_HEADER := $(call compute-api-name,$(LIB_NAME)))
	@echo "[$* => $(MOD_DIR)] Building library for \"$(MOD_DIR)\" (lib$(LIB_NAME).a)"
	$(SILENT_PREFIX)$(MAKE) -C $(MOD_DIR) static-lib \
		MODE=$(MODE) \
		VERBOSE=$(VERBOSE) \
		LIB_NAME=$(LIB_NAME) \
		EXTRA_CFLAGS="-DASSET_PATH=\\\"$(MOD_DIR)/assets/\\\""
	$(SILENT_PREFIX)mkdir -p $(LIB_DIR)
	$(SILENT_PREFIX)if cmp -s $(MOD_DIR)/build/lib/lib$(LIB_NAME).a $@ 2>/dev/null; then \
		echo "  lib$(LIB_NAME).a unchanged"; \
	else \
		echo "  Updating lib$(LIB_NAME).a"; \
		cp $(MOD_DIR)/build/lib/lib$(LIB_NAME).a $@; \
	fi
	$(SILENT_PREFIX)mkdir -p $(FIRSTPARTY_API_DIR)
	$(SILENT_PREFIX)if [ -f "$(MOD_DIR)/include/$(API_HEADER)" ]; then \
		echo "  Updating API header: $(API_HEADER)"; \
		cp "$(MOD_DIR)/include/$(API_HEADER)" "$(FIRSTPARTY_API_DIR)/"; \
	else \
		echo "  Warning: $(API_HEADER) not found in $(MOD_DIR)/include/"; \
	fi

# Normal incremental build of lobby (client) executable
client: libs
	@echo "Building lobby executable (if needed)..."
	$(SILENT_PREFIX)mkdir -p $(BIN_DIR)
	$(SILENT_PREFIX)$(MAKE) -C lobby \
		MODE=$(MODE) \
		VERBOSE=$(VERBOSE) \
		BIN_DIR=../$(BIN_DIR) \
		EXTRA_CFLAGS="-DASSET_PATH=\\\"lobby/assets/\\\"" \
		EXTRA_LDFLAGS="$(LIBS_REL)"

# Build server binary (links against all libs)
server: libs | $(BIN_DIR)
	@echo "Building server executable..."
	$(SILENT_PREFIX)$(MAKE) -C reseau MODE=$(MODE) VERBOSE=$(VERBOSE) \
		EXTRA_LDFLAGS="-L../build/lib -Wl,--start-group \
			-llobby -lbingo -lkingforfour -lchess -lrubik -lfirstparty \
			-Wl,--end-group -lm"
	$(SILENT_PREFIX)install -m 755 reseau/build/bin/server$(EXE_EXT) $(BIN_DIR)/server$(EXE_EXT)

# Run targets
run-client: client
	$(SILENT_PREFIX)if [ -f "$(BIN_DIR)/client$(EXE_EXT)" ]; then \
		echo "===> Starting client..."; \
		$(BIN_DIR)/client$(EXE_EXT); \
	else \
		echo "No executable found: $(BIN_DIR)/client$(EXE_EXT)"; \
		echo "Run 'make rebuild' first."; \
	fi

run-server: server
	$(SILENT_PREFIX)if [ -f "$(BIN_DIR)/server$(EXE_EXT)" ]; then \
		echo "===> Starting server..."; \
		$(BIN_DIR)/server$(EXE_EXT); \
	else \
		echo "No executable found: $(BIN_DIR)/server$(EXE_EXT)"; \
		echo "Run 'make rebuild' first."; \
	fi

run-multi: all
	$(SILENT_PREFIX)./test-multi.sh 2 $(MODE)

# ==============================================================================
# Test Targets
# ==============================================================================

tests:
	$(SILENT_PREFIX)for dir in $(MODULES); do \
		if [ -d "$$dir" ] && [ -f "$$dir/Makefile" ]; then \
			echo "Building tests in $$dir ..."; \
			$(MAKE) -C "$$dir" tests MODE=$(MODE) VERBOSE=$(VERBOSE) || exit 1; \
		fi; \
	done
	@echo ""
	@echo "All test binaries built."

run-tests: tests
	@echo ""
	@echo "Running all tests across modules..."
	@echo "====================================================="
	$(SILENT_PREFIX)all_passed=1; \
	total_modules=0; \
	failed_modules=0; \
	for dir in $(MODULES); do \
		if [ -d "$$dir" ] && [ -f "$$dir/Makefile" ]; then \
			echo "-> Module: $$dir"; \
			$(MAKE) -C "$$dir" run-tests MODE=$(MODE) VERBOSE=$(VERBOSE) \
				|| { all_passed=0; failed_modules=$$((failed_modules+1)); }; \
			total_modules=$$((total_modules+1)); \
			echo ""; \
		fi; \
	done
	@echo "-----------------------------------------------"
	$(SILENT_PREFIX)if [ $$all_passed -eq 1 ]; then \
		echo "ALL TESTS PASSED across $$total_modules module(s)."; \
	else \
		echo "SOME TESTS FAILED ($$failed_modules / $$total_modules modules had failures)."; \
		exit 1; \
	fi

# ==============================================================================
# Clean and Rebuild Targets
# ==============================================================================

clean-libs:
	$(SILENT_PREFIX)for dir in $(MODULES); do \
		if [ -d "$$dir" ] && [ -f "$$dir/Makefile" ]; then \
			$(MAKE) -C "$$dir" clean VERBOSE=$(VERBOSE); \
		fi; \
	done
	$(SILENT_PREFIX)rm -rf $(LIB_DIR)

clean-bins:
	$(SILENT_PREFIX)rm -rf $(BIN_DIR)

clean-client:
	$(SILENT_PREFIX)rm -rf $(BIN_DIR)/client$(EXE_EXT)

clean-server:
	$(SILENT_PREFIX)rm -rf $(BIN_DIR)/server$(EXE_EXT)

clean:
	$(SILENT_PREFIX)rm -rf $(BUILD_DIR)

clean-all: clean
	$(SILENT_PREFIX)for dir in $(MODULES); do \
		if [ -d "$$dir" ] && [ -f "$$dir/Makefile" ]; then \
			echo "Cleaning $$dir ..."; \
			$(MAKE) -C "$$dir" clean VERBOSE=$(VERBOSE); \
		fi; \
	done

rebuild: clean-all all

rebuild-libs: clean-libs libs

rebuild-client: clean-client client
rebuild-server: clean-server server

rebuild-tests: clean tests

# ==============================================================================
# Documentation Targets
# ==============================================================================

# Build root documentation (English)
docs: docs-root

docs-root:
	@./generate-root-docs.sh

# Translate documentation to specified languages
# Depends on docs-root to ensure English version exists first
docs-translate: docs
	@if [ -z "$(LANG)" ]; then \
		echo "Usage: make docs-translate LANG=fr,de,es"; \
		exit 1; \
	fi
	@./translate-root-docs.sh $(LANG)

# Remove all generated translations and related files
docs-translate-free:
	$(SILENT_PREFIX)rm -rf docs/doxygen/html-* docs/doxygen/src-*
	$(SILENT_PREFIX)find docs/doxygen -maxdepth 1 -name 'Doxyfile.*' -delete
	@echo "Translated documentation removed."

# ==============================================================================
# Help Target
# ==============================================================================

help:
	@echo "Usage: make [OPTIONS] [TARGET]"
	@echo ""
	@echo "TARGETS:"
	@echo "    help                         Print this help message"
	@echo "    all                          Build all libraries + lobby client + server"
	@echo "    client                       Build lobby client executable (depends on libs)"
	@echo "    server                       Build game server executable"
	@echo "    libs                         Build module static libraries (incremental)"
	@echo ""
	@echo "    run-client                   Run the lobby client"
	@echo "    run-server                   Run the game server"
	@echo "    run-multi                    Run multi-instance test script"
	@echo ""
	@echo "    tests                        Build test executables in all modules"
	@echo "    run-tests                    Run all tests across modules"
	@echo ""
	@echo "    rebuild                      Clean everything and perform full rebuild"
	@echo "    rebuild-libs                 Clean and rebuild only libraries"
	@echo "    rebuild-client               Clean and rebuild only client"
	@echo "    rebuild-server               Clean and rebuild only server"
	@echo "    rebuild-tests                Clean and rebuild tests"
	@echo ""
	@echo "    clean                        Remove root build/ directory only"
	@echo "    clean-all                    Clean root + all submodule build artifacts"
	@echo "    clean-libs                   Clean only libraries (root + subdirs)"
	@echo "    clean-bins                   Clean only bin/ directory"
	@echo "    clean-client                 Clean only client executable"
	@echo "    clean-server                 Clean only server executable"
	@echo ""
	@echo "    docs / docs-root             Build root documentation (English)"
	@echo "    docs-translate LANG=...      Translate docs (e.g. LANG=fr,de,es)"
	@echo "    docs-translate-free          Remove all translated documentation"
	@echo ""
	@echo "OPTIONS:"
	@echo "    MODE=<mode>      release | debug | strict-debug | clang-debug | valgrind-debug"
	@echo "    VERBOSE=1        Show full command output (default: silent)"
	@echo ""
	@echo "Notes:"
	@echo "  - Libraries are built lazily using timestamp comparison"
	@echo "  - Server links all libraries using --start-group / --end-group"
	@echo "  - clean affects only root build/; use clean-all for a full reset"
	@echo "  - Output goes to: build/lib/lib*.a and build/bin/{client|server}"

.PHONY: all client server libs \
        run-client run-server run-multi \
        tests run-tests \
        clean clean-all clean-libs clean-bins clean-client clean-server \
        rebuild rebuild-libs rebuild-client rebuild-server rebuild-tests \
        docs docs-root docs-translate docs-translate-free help