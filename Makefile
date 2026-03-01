# ───────────────────────────────────────────────────────────────
# Top-Level Makefile ─ Project build system
#
#     Main targets: see 'make help' for the full list and descriptions
#     Modes:        release, debug, strict-debug, clang-debug, valgrind-debug
#
#     Quick usage        ->  make help
#
# Author: Fshimi Hawlk <https://github.com/Fshimi-Hawlk>
# ───────────────────────────────────────────────────────────────

# ───────────────────────────────────────────────────────────────
# Configuration
# ───────────────────────────────────────────────────────────────

MODE ?= release
VERBOSE ?= 0
MAIN_NAME ?= main

# Verbose mode (default silent)
ifeq ($(VERBOSE),1)
	SILENT_PREFIX :=
else
	SILENT_PREFIX := @
endif

# Fait Physique : Fige le chemin absolu de la racine du projet
# Garantit que les sous-Makefiles (lobby, reseau) trouvent toujours les API
ROOT_DIR := $(CURDIR)

# Suppress subdirectory messages
MAKEFLAGS += --no-print-directory

# Excluded directories
EXCLUDED_DIRS := \
	$(ROOT_DIR)/assets \
	$(ROOT_DIR)/build \
	$(ROOT_DIR)/docs \
	$(ROOT_DIR)/firstparty \
	$(ROOT_DIR)/lobby \
	$(ROOT_DIR)/logs \
	$(ROOT_DIR)/thirdparty

# Detect modules (normalize by removing trailing /)
MODULES := $(patsubst %/,%,$(wildcard */))
MODULES := $(filter-out $(EXCLUDED_DIRS), $(MODULES))

SUBDIRS := $(MODULES) $(ROOT_DIR)/lobby

FIRSTPARTY_API_DIR := $(ROOT_DIR)/firstparty/APIs

# Root-level directories
BUILD_DIR := $(ROOT_DIR)/build
LIB_DIR   := $(BUILD_DIR)/lib
BIN_DIR   := $(BUILD_DIR)/bin

# Computed lib names (flattened lowercase, no - or _)
define compute-lib-name
$(shell echo '$(1)' | tr '[:upper:]' '[:lower:]' | tr -d '_-')
endef

define compute-api-name
$(shell echo '$(1)API.h')
endef

# Absolute lib paths (used for dependency tracking)
LIBS := $(foreach mod,$(MODULES),$(LIB_DIR)/lib$(call compute-lib-name,$(mod)).a)

# Lib paths for linking (relative from lobby/ -> ../../build/lib/...)
LIBS_REL := $(foreach lib,$(LIBS),../$(lib))

# ───────────────────────────────────────────────────────────────
# Main Targets
# ───────────────────────────────────────────────────────────────

all: bin

# Lazy build of module libraries (only if sources changed)
libs: $(LIBS)

$(LIB_DIR)/lib%.a:
	$(eval MOD_DIR := $(filter %$*,$(MODULES)))
	$(eval LIB_NAME := $(call compute-lib-name,$(MOD_DIR)))
	$(eval API_HEADER := $(call compute-api-name,$(LIB_NAME)))
	@echo "Building library for $(MOD_DIR)"
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

# Normal incremental build of lobby executable
bin: libs
	@echo "Building lobby executable (if needed)..."
	$(SILENT_PREFIX)mkdir -p $(BIN_DIR)
	$(SILENT_PREFIX)$(MAKE) -C lobby \
		MODE=$(MODE) \
		VERBOSE=$(VERBOSE) \
		BIN_DIR=../$(BIN_DIR) \
		EXTRA_CFLAGS="-DASSET_PATH=\\\"lobby/assets/\\\"" \
		EXTRA_LDFLAGS="$(LIBS_REL)"

run-server: libs
	$(SILENT_PREFIX)echo "===> Starting Server..."
	$(SILENT_PREFIX)cd reseau && ./build/bin/server

run-client: bin
	@if [ -f $(BIN_DIR)/$(MAIN_NAME) ]; then \
		$(SILENT_PREFIX)echo "===> Starting Client..."
		$(BIN_DIR)/$(MAIN_NAME); \
	else \
		echo "No executable found at $(BIN_DIR)/$(MAIN_NAME)"; \
		echo "Run 'make rebuild' first."; \
	fi

# ───────────────────────────────────────────────────────────────
# Test Targets
# ───────────────────────────────────────────────────────────────

tests:
	$(SILENT_PREFIX)for dir in $(SUBDIRS); do \
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
	@echo "───────────────────────────────────────────────"
	$(SILENT_PREFIX)all_passed=1; \
	total_modules=0; \
	failed_modules=0; \
	for dir in $(SUBDIRS); do \
		if [ -d "$$dir" ] && [ -f "$$dir/Makefile" ]; then \
			echo "-> Module: $$dir"; \
			$(MAKE) -C "$$dir" run-tests MODE=$(MODE) VERBOSE=$(VERBOSE) \
				|| { all_passed=0; failed_modules=$$((failed_modules+1)); }; \
			total_modules=$$((total_modules+1)); \
			echo ""; \
		fi; \
	done
	@echo "───────────────────────────────────────────────"
	$(SILENT_PREFIX)if [ $$all_passed -eq 1 ]; then \
		echo "ALL TESTS PASSED across $$total_modules module(s)."; \
	else \
		echo "SOME TESTS FAILED ($$failed_modules / $$total_modules modules had failures)."; \
		exit 1; \
	fi

# ───────────────────────────────────────────────────────────────
# Clean and Rebuild
# ───────────────────────────────────────────────────────────────

clean-libs:
	$(SILENT_PREFIX)for dir in $(SUBDIRS); do \
		if [ -d "$$dir" ] && [ -f "$$dir/Makefile" ]; then \
			$(MAKE) -C "$$dir" clean VERBOSE=$(VERBOSE); \
		fi; \
	done
	$(SILENT_PREFIX)rm -rf $(LIB_DIR)

clean-bin:
	$(SILENT_PREFIX)rm -rf $(BIN_DIR)

clean:
	$(SILENT_PREFIX)rm -rf $(BUILD_DIR)

clean-all: clean
	$(SILENT_PREFIX)echo "===> Cleaning Architecture..."
	$(SILENT_PREFIX)for dir in $(SUBDIRS); do \
		if [ -d "$$dir" ] && [ -f "$$dir/Makefile" ]; then \
			rm -f $(FIRSTPARTY_API_DIR)/$(dir)API.h
			echo "Cleaning $$dir ..."; \
			$(MAKE) -C "$$dir" clean VERBOSE=$(VERBOSE); \
		fi; \
	done

rebuild: clean-all all

rebuild-libs: clean-libs libs

rebuild-bin: clean-bin bin

rebuild-tests: clean tests

docs:
	@./generate-root-docs.sh

# ───────────────────────────────────────────────────────────────
# Help
# ───────────────────────────────────────────────────────────────

help:
	@echo "Usage: make [OPTIONS] [TARGET]"
	@echo ""
	@echo "TARGETS:"
	@echo "    help             Print this help message"
	@echo "    all              Build libs + lobby executable"
	@echo "    rebuild          Clean everything (root + subdirs) and rebuild all"
	@echo "    clean            Remove root-level build/ folder only"
	@echo "    clean-all        Remove root build/ + clean every submodule"
	@echo "    clean-libs       Clean only libraries (root + subdirs)"
	@echo "    clean-bin        Clean only lobby executable"
	@echo "    libs             Build module static libs if needed"
	@echo "    rebuild-libs     Clean libraries and force rebuild"
	@echo "    bin              Build lobby executable if needed"
	@echo "    rebuild-bin      Force rebuild lobby executable only"
	@echo "    run-bin          Run the lobby executable"
	@echo "    tests            Build all test executables"
	@echo "    rebuild-tests    Clean and rebuild test executables"
	@echo "    run-tests        Run all tests"
	@echo "    docs             Build the root documentation"
	@echo ""
	@echo "OPTIONS:"
	@echo "    MODE=<str>       release | debug | strict-debug | clang-debug | valgrind-debug"
	@echo "    MAIN_NAME=<str>  Custom executable name (default: main)"
	@echo "    VERBOSE=1        Show all commands (default: silent)"
	@echo ""
	@echo "Notes:"
	@echo "  - libs are built lazily (only when sources change)"
	@echo "  - rebuild-bin forces relinking of the lobby executable"
	@echo "  - clean only affects root build/ — use clean-all for full reset"
	@echo "  - Output: build/lib/lib*.a and build/bin/$(MAIN_NAME)"

.PHONY: all libs bin rebuild-bin run-server run-lobby tests run-tests clean clean-all clean-libs clean-bin rebuild rebuild-libs rebuild-tests docs help
