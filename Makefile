# ==============================================================================
# Top-Level Makefile - Project build system
#
#     Main targets: see 'make help' for the full list and descriptions
#     Modes:        release, debug, strict-debug, clang-debug, valgrind-debug
#
#     Quick usage   ->  make help
#
# Author: Fshimi Hawlk <https://github.com/Fshimi-Hawlk>
# ==============================================================================

# External libraries
RAYLIB_VERSION := 5.5
UNAME_S := $(shell uname -s)

# OS and specific flags
ifeq ($(UNAME_S),Linux)
    OS := linux
    LDFLAGS_PLATFORM := -lGL -lm -lpthread -ldl -lrt -lX11
    RAYLIB_LIB_DIR := ./thirdparty/libs/raylib-$(RAYLIB_VERSION)_linux_amd64
endif
ifeq ($(UNAME_S),Darwin)
    OS := darwin
    LDFLAGS_PLATFORM := -framework CoreVideo -framework IOKit -framework Cocoa -framework GLUT -framework OpenGL
    RAYLIB_LIB_DIR := ./thirdparty/libs/raylib-$(RAYLIB_VERSION)_macos
endif
ifeq ($(findstring MINGW,$(UNAME_S)),MINGW)
    OS := mingw
    EXE_EXT := .exe
    LDFLAGS_PLATFORM := -lopengl32 -lgdi32 -lwinmm
    RAYLIB_LIB_DIR := ./thirdparty/libs/raylib-$(RAYLIB_VERSION)_win64_mingw
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

# Suppress subdirectory messages
MAKEFLAGS += --no-print-directory

# Directories to exclude when discovering modules
EXCLUDED_DIRS := \
    assets       \
    build        \
    docs         \
    jeux         \
    logs         \
    thirdparty   \
    tui-ver

# Detect modules (normalize by removing trailing /)
ROOT_MODULES := $(patsubst %/,%,$(wildcard */))
ROOT_MODULES := $(filter-out $(EXCLUDED_DIRS), $(ROOT_MODULES))

# jeux/ sub-project modules (e.g. jeux/tetris, jeux/solitaire, ...)
JEUX_MODULES := $(patsubst %/,%,$(wildcard jeux/*/))

MODULES := $(ROOT_MODULES) $(JEUX_MODULES)

FIRSTPARTY_API_DIR := firstparty/APIs

# Root-level directories
BUILD_DIR := build
LIB_DIR   := $(BUILD_DIR)/lib
BIN_DIR   := $(BUILD_DIR)/bin

# Computed lib names (flattened lowercase, no - or _)
# Uses notdir to handle paths like jeux/tetris → tetris
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
$(BIN_DIR) $(LIB_DIR) $(OBJ_DIR) $(API_DIR):
	$(SILENT_PREFIX)mkdir -p $@

# Lazy build of module libraries (only if sources changed)
libs: $(LIBS)

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

# Don't know which version to keep...

# Module build rule
# module-%: | $(LIB_DIR) $(API_DIR)
# 	@echo "Building static library -> $* (lib$*.a)"
# 	$(SILENT_PREFIX)$(MAKE) -j1 -C $* static-lib MODE=$(MODE) VERBOSE=$(VERBOSE) LIB_NAME=$(subst -,,$*) EXTRA_CFLAGS="-I$(CURDIR)/firstparty -I$(CURDIR)/thirdparty"
# 	$(SILENT_PREFIX)if [ "$*" = "chess" ]; then \
# 		$(MAKE) -j1 -C chess static-lib MODE=$(MODE) VERBOSE=$(VERBOSE) LIB_NAME=chess MAIN_NAME=chess_core EXTRA_CFLAGS="-I$(CURDIR)/firstparty -I$(CURDIR)/thirdparty"; \
# 	fi
# 	$(SILENT_PREFIX)if cmp -s "$*/build/lib/lib$(subst -,,$*).a" "$(LIB_DIR)/lib$(subst -,,$*).a" 2>/dev/null; then \
# 		echo "  lib$(subst -,,$*).a unchanged"; \
# 	else \
# 		echo "  Updating lib$(subst -,,$*).a"; \
# 		cp "$*/build/lib/lib$(subst -,,$*).a" "$(LIB_DIR)/lib$(subst -,,$*).a"; \
# 	fi
# 	$(SILENT_PREFIX)mkdir -p $(API_DIR)
# 	$(SILENT_PREFIX)if [ -f "$*/include/$*API.h" ]; then \
# 		echo "  Updating API header -> $*API.h"; \
# 		cp "$*/include/$*API.h" "$(API_DIR)/"; \
# 	elif [ -f "$*/include/$(shell echo $* | sed -E 's/(-[a-z])/\U\1/g' | sed 's/-//g')API.h" ]; then \
# 		api_h="$(shell echo $* | sed -E 's/(-[a-z])/\U\1/g' | sed 's/-//g')API.h"; \
# 		echo "  Updating API header -> $$api_h"; \
# 		cp "$*/include/$$api_h" "$(API_DIR)/"; \
# 	else \
# 		echo "  Warning: API header not found for $*"; \
# 	fi

# # Specific module targets
# modules: $(foreach mod,$(ORDERED_MODULES),module-$(mod))

# Normal incremental build of lobby executable
client: libs
	@echo "Building lobby executable (if needed)..."
	$(SILENT_PREFIX)mkdir -p $(BIN_DIR)
	$(SILENT_PREFIX)$(MAKE) -C lobby \
		MODE=$(MODE) \
		VERBOSE=$(VERBOSE) \
		BIN_DIR=../$(BIN_DIR) \
		EXTRA_CFLAGS="-DASSET_PATH=\\\"lobby/assets/\\\"" \
		EXTRA_LDFLAGS="$(LIBS_REL)"

# Build server binary
server: modules | $(BIN_DIR)
	@echo "Building server executable..."
	$(SILENT_PREFIX)$(MAKE) -C reseau MODE=$(MODE) VERBOSE=$(VERBOSE) EXTRA_LDFLAGS="-L../build/lib -Wl,--start-group -llobby -lbingo -lkingforfour -lchess -lrubik -lfirstparty -Wl,--end-group -lm"
	$(SILENT_PREFIX)install -m 755 reseau/build/bin/server$(EXE_EXT) $(BIN_DIR)/server$(EXE_EXT)

run-client: bin
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
# Clean and Rebuild
# ==============================================================================

clean-libs:
	$(SILENT_PREFIX)for dir in $(MODULES); do \
		if [ -d "$$dir" ] && [ -f "$$dir/Makefile" ]; then \
			$(MAKE) -C "$$dir" clean VERBOSE=$(VERBOSE); \
		fi; \
	done
	$(SILENT_PREFIX)rm -rf $(LIB_DIR)

clean-exe:
	$(SILENT_PREFIX)rm -rf $(BIN_DIR)

clean-client:
	$(BIN_DIR)/client$(EXE_EXT); \

clean-server:
	$(BIN_DIR)/server$(EXE_EXT); \


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
# Documentation
# ==============================================================================

# FIX: target renamed 'docs' (was announced as 'docs-root' in help but didn't exist).
# Both names are now valid aliases.
docs: docs-root

docs-root:
	@./generate-root-docs.sh

# FIX: guard on LANG — displays a clear message if the variable is not provided.
# docs-translate depends on docs — ensures html/ (EN doc) exists before translating.
# Without this, the English button in index.php points to a non-existent folder.
docs-translate: docs
	@if [ -z "$(LANG)" ]; then \
		echo "Usage: make docs-translate LANG=fr,de,es"; \
		exit 1; \
	fi
	@./translate-root-docs.sh $(LANG)

# Removes all generated translations: html-XX/, src-XX/ folders
# and corresponding Doxyfile.<lang> files (all except the base EN Doxyfile).
docs-translate-free:
	$(SILENT_PREFIX)rm -rf docs/doxygen/html-* docs/doxygen/src-*
	$(SILENT_PREFIX)find docs/doxygen -maxdepth 1 -name 'Doxyfile.*' -delete
	@echo "Translated documentation removed."

# ==============================================================================
# Help
# ==============================================================================

help:
	@echo "Usage: make [OPTIONS] [TARGET]"
	@echo ""
	@echo "TARGETS:"
	@echo "    help                         Print this help message"
	@echo "    all                          Build libs + lobby executable"
	@echo "    rebuild                      Clean everything (root + subdirs) and rebuild all"
	@echo "    clean                        Remove root-level build/ folder only"
	@echo "    clean-all                    Remove root build/ + clean every submodule"
	@echo "    clean-libs                   Clean only libraries (root + subdirs)"
	@echo "    clean-exe                    Clean only lobby executable"
	@echo "    libs                         Build module static libs if needed"
	@echo "    rebuild-libs                 Clean libraries and force rebuild"
	@echo "    bin                          Build lobby executable if needed"
	@echo "    run-client                   Run the lobby client"
	@echo "    run-server                   Run the game server"
	@echo "    tests                        Build all test executables"
	@echo "    rebuild-tests                Clean and rebuild test executables"
	@echo "    run-tests                    Run all tests"
	@echo "    docs / docs-root             Build the root documentation (EN)"
	@echo "    docs-translate LANG=fr,de    Translate documentation to given languages"
	@echo "    docs-translate-free          Remove all translated documentation folders"
	@echo ""
	@echo "OPTIONS:"
	@echo "    MODE=<str>       release | debug | strict-debug | clang-debug | valgrind-debug"
	@echo "    VERBOSE=1        Show all commands (default: silent)"
	@echo ""
	@echo "Notes:"
	@echo "  - libs are built lazily (only when sources change)"
	@echo "  - rebuild-exe forces relinking of the lobby executable"
	@echo "  - clean only affects root build/ - use clean-all for full reset"
	@echo "  - Output: build/lib/lib*.a and build/bin/{client|server}"

.PHONY: all libs bin rebuild-exe run-exe tests run-tests \
        clean clean-all clean-libs clean-exe \
        rebuild rebuild-libs rebuild-tests \
        docs docs-root docs-translate docs-translate-free help