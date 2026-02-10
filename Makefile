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

# Suppress subdirectory messages
MAKEFLAGS += --no-print-directory

# Excluded directories
EXCLUDED_DIRS := assets build docs firstparty lobby logs thirdparty

# Detect modules (normalize by removing trailing /)
MODULES := $(patsubst %/,%,$(wildcard */))
MODULES := $(filter-out $(EXCLUDED_DIRS), $(MODULES))

SUBDIRS := $(MODULES) lobby

FIRSTPARTY_API_DIR := firstparty/APIs

# Root-level directories
BUILD_DIR := build
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
	$(SILENT_PREFIX)echo "Building library for $(MOD_DIR) if needed..."
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
	$(SILENT_PREFIX)echo "Building lobby executable (if needed)..."
	$(SILENT_PREFIX)mkdir -p $(BIN_DIR)
	$(SILENT_PREFIX)$(MAKE) -C lobby \
		MODE=$(MODE) \
		VERBOSE=$(VERBOSE) \
		BIN_DIR=../$(BIN_DIR) \
		EXTRA_CFLAGS="-DASSET_PATH=\\\"lobby/assets/\\\"" \
		EXTRA_LDFLAGS="$(LIBS_REL)"

# Force rebuild of lobby executable only (removes exe first)
rebuild-exe: libs
	$(SILENT_PREFIX)echo "Force rebuilding lobby executable..."
	$(SILENT_PREFIX)rm -f $(BIN_DIR)/$(MAIN_NAME)
	$(SILENT_PREFIX)mkdir -p $(BIN_DIR)
	$(SILENT_PREFIX)$(MAKE) -C lobby \
		MODE=$(MODE) \
		VERBOSE=$(VERBOSE) \
		BIN_DIR=../$(BIN_DIR) \
		EXTRA_CFLAGS="-DASSET_PATH=\\\"lobby/assets/\\\"" \
		EXTRA_LDFLAGS="$(LIBS_REL)" \
		rebuild

# Run the lobby executable
run-exe:
	@if [ -f $(BIN_DIR)/$(MAIN_NAME) ]; then \
		$(BIN_DIR)/$(MAIN_NAME); \
	else \
		echo "No executable found at $(BIN_DIR)/$(MAIN_NAME)"; \
		echo "Run 'make bin' or 'make rebuild-exe' first."; \
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
	$(SILENT_PREFIX)echo ""
	$(SILENT_PREFIX)echo "All test binaries built."

run-tests: tests
	$(SILENT_PREFIX)echo ""
	$(SILENT_PREFIX)echo "Running all tests across modules..."
	$(SILENT_PREFIX)echo "───────────────────────────────────────────────"
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
	$(SILENT_PREFIX)echo "───────────────────────────────────────────────"
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

clean-exe:
	$(SILENT_PREFIX)rm -rf $(BIN_DIR)

clean:
	$(SILENT_PREFIX)rm -rf $(BUILD_DIR)

clean-all: clean
	$(SILENT_PREFIX)for dir in $(SUBDIRS); do \
		if [ -d "$$dir" ] && [ -f "$$dir/Makefile" ]; then \
			echo "Cleaning $$dir ..."; \
			$(MAKE) -C "$$dir" clean VERBOSE=$(VERBOSE); \
		fi; \
	done

rebuild: clean-all all

rebuild-libs: clean-libs libs

rebuild-exe: clean-exe bin

rebuild-tests: clean tests

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
	@echo "    clean-exe        Clean only lobby executable"
	@echo "    libs             Build module static libs if needed"
	@echo "    rebuild-libs     Clean libraries and force rebuild"
	@echo "    bin              Build lobby executable if needed"
	@echo "    rebuild-exe      Force rebuild lobby executable only"
	@echo "    run-exe          Run the lobby executable"
	@echo "    tests            Build all test executables"
	@echo "    rebuild-tests    Clean and rebuild test executables"
	@echo "    run-tests        Run all tests"
	@echo ""
	@echo "OPTIONS:"
	@echo "    MODE=<str>       release | debug | strict-debug | clang-debug | valgrind-debug"
	@echo "    MAIN_NAME=<str>  Custom executable name (default: main)"
	@echo "    VERBOSE=1        Show all commands (default: silent)"
	@echo ""
	@echo "Notes:"
	@echo "  - libs are built lazily (only when sources change)"
	@echo "  - rebuild-exe forces relinking of the lobby executable"
	@echo "  - clean only affects root build/ — use clean-all for full reset"
	@echo "  - Output: build/lib/lib*.a and build/bin/$(MAIN_NAME)"

.PHONY: all libs bin rebuild-exe run-exe tests run-tests clean clean-all clean-libs clean-exe rebuild rebuild-libs rebuild-tests help