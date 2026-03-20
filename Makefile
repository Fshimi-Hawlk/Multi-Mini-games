# ╭────────────────────────────────────────────────────────────────────────╮
# │                        Project Root Makefile                           │
# │                                                                        │
# │  Build system for multi-module C/C++ project with static libraries     │
# │  and centralized lobby executable                                      │
# │                                                                        │
# │  Main entry points:                                                    │
# │    • make help           ─ show all available targets & options        │
# │    • make all            ─ build libraries + lobby executable          │
# │    • make rebuild        ─ full clean then rebuild everything          │
# │    • make clean-all      ─ deep clean (build dir + modules + headers)  │
# │                                                                        │
# │  Build modes: release, debug, strict-debug, clang-debug, valgrind-debug│
# │  Verbose output: VERBOSE=1                                             │
# │                                                                        │
# │  Author:   Fshimi Hawlk                                                │
# │  GitHub:   https://github.com/Fshimi-Hawlk                             │
# │  Updated:  March 2025                                                  │
# ╰────────────────────────────────────────────────────────────────────────╯


# ───────────────────────────────────────────────────────────────
# Configuration
# ───────────────────────────────────────────────────────────────

MODE          ?= release
VERBOSE       ?= 0
MAIN_NAME     ?= main

# Verbose control
ifeq ($(VERBOSE),1)
    SILENT_PREFIX :=
else
    SILENT_PREFIX := @
endif

ROOT_DIR      := $(CURDIR)
MAKEFLAGS     += --no-print-directory

# Directories to exclude when discovering modules
EXCLUDED_DIRS := \
    assets       \
    build        \
    docs         \
    firstparty   \
    lobby        \
    logs         \
    thirdparty

# Discover module directories (exclude listed ones)
MODULES := $(patsubst %/,%,$(wildcard */))
MODULES := $(filter-out $(EXCLUDED_DIRS),$(MODULES))

SUBDIRS       := $(MODULES) lobby

FIRSTPARTY_API_DIR := $(ROOT_DIR)/firstparty/APIs

BUILD_DIR     := $(ROOT_DIR)/build
LIB_DIR       := $(BUILD_DIR)/lib
BIN_DIR       := $(BUILD_DIR)/bin

# ───────────────────────────────────────────────────────────────
# Computed names & paths
# ───────────────────────────────────────────────────────────────

# Transform module directory name -> library name (lowercase, no _ or -)
define compute-lib-name
$(shell echo '$(1)' | tr '[:upper:]' '[:lower:]' | tr -d '_-')
endef

# Convert directory name (with possible dashes) -> lowerCamelCase for API header
# bingo          -> bingoAPI.h
# king-for-four  -> kingForFourAPI.h
# reseau         -> reseauAPI.h
define to-lower-camel
$(shell echo '$(1)' | sed -E 's/[-_]([a-z])/\U\1/g' | sed 's/^[A-Z]/\l&/')
endef

define compute-api-name
$(call to-lower-camel,$(1))API.h
endef

$(LIB_DIR):
	$(SILENT_PREFIX)mkdir -p $@

# Absolute library paths
LIBS := $(foreach mod,$(MODULES),$(LIB_DIR)/lib$(call compute-lib-name,$(mod)).a)

# Relative paths from lobby/ to libs (for linker -L or -l usage)
LIBS_REL := $(foreach lib,$(LIBS),../$(lib))

define build-module-lib-rule
$(LIB_DIR)/lib$(call compute-lib-name,$(1)).a : | $(LIB_DIR)
	@echo "Building static library -> $(1) (lib$(call compute-lib-name,$(1)).a)"
	$(SILENT_PREFIX)$(MAKE) -C $(1) static-lib \
		MODE=$(MODE) \
		VERBOSE=$(VERBOSE) \
		LIB_NAME=$(call compute-lib-name,$(1)) \
		EXTRA_CFLAGS="-DASSET_PATH=\\\"$(1)/assets/\\\""
	$(SILENT_PREFIX)if cmp -s "$(1)/build/lib/lib$(call compute-lib-name,$(1)).a" "$$@" 2>/dev/null; then \
		echo "  lib$(call compute-lib-name,$(1)).a unchanged"; \
	else \
		echo "  Updating lib$(call compute-lib-name,$(1)).a"; \
		cp "$(1)/build/lib/lib$(call compute-lib-name,$(1)).a" "$$@"; \
	fi
	$(SILENT_PREFIX)mkdir -p $(FIRSTPARTY_API_DIR)
	$(SILENT_PREFIX)if [ -f "$(1)/include/$(call compute-api-name,$(1))" ]; then \
		echo "  Updating API header -> $(call compute-api-name,$(1))"; \
		cp "$(1)/include/$(call compute-api-name,$(1))" "$(FIRSTPARTY_API_DIR)/"; \
	else \
		echo "  Warning: $(call compute-api-name,$(1)) not found in $(1)/include/"; \
	fi
endef

$(foreach mod,$(MODULES),$(eval $(call build-module-lib-rule,$(mod))))

# ───────────────────────────────────────────────────────────────
# Main targets
# ───────────────────────────────────────────────────────────────

all: bin

libs: $(LIBS)

bin: libs
	@echo "Building lobby executable..."
	$(SILENT_PREFIX)mkdir -p $(BIN_DIR)
	$(SILENT_PREFIX)$(MAKE) -C lobby \
		MODE=$(MODE) VERBOSE=$(VERBOSE) \
		BIN_DIR=../$(BIN_DIR) \
		EXTRA_CFLAGS="-DASSET_PATH=\\\"lobby/assets/\\\"" \
		EXTRA_LDFLAGS="$(LIBS_REL)"

run-server: libs
	@echo "===> Starting server..."
	$(SILENT_PREFIX)cd reseau && ./build/bin/server

run-client: bin
	@if [ -f "$(BIN_DIR)/$(MAIN_NAME)" ]; then \
		echo "===> Starting client..."; \
		$(BIN_DIR)/$(MAIN_NAME); \
	else \
		echo "No executable found: $(BIN_DIR)/$(MAIN_NAME)"; \
		echo "Run 'make rebuild' first."; \
	fi

# ───────────────────────────────────────────────────────────────
# Testing
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
	@echo "Running tests across modules..."
	@echo "───────────────────────────────────────────────"
	$(SILENT_PREFIX)all_passed=1; failed=0; total=0; \
	for dir in $(SUBDIRS); do \
		if [ -d "$$dir" ] && [ -f "$$dir/Makefile" ]; then \
			echo "-> $$dir"; \
			$(MAKE) -C "$$dir" run-tests MODE=$(MODE) VERBOSE=$(VERBOSE) \
				|| { all_passed=0; failed=$$((failed+1)); }; \
			total=$$((total+1)); echo ""; \
		fi; \
	done
	@echo "───────────────────────────────────────────────"
	$(SILENT_PREFIX)if [ $$all_passed -eq 1 ]; then \
		echo "ALL TESTS PASSED ($$total module(s))"; \
	else \
		echo "SOME TESTS FAILED ($$failed / $$total modules)"; \
		exit 1; \
	fi

# ───────────────────────────────────────────────────────────────
# Clean & rebuild
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

clean-api-headers:
	$(SILENT_PREFIX)for dir in $(SUBDIRS); do \
		if [ -d "$$dir" ] && [ -f "$$dir/Makefile" ]; then \
			module_name=$$(basename "$$dir"); \
			api_file="$(FIRSTPARTY_API_DIR)/$$(echo "$$module_name" | sed -E 's/[-_]([a-z])/\U\1/g' | sed 's/^[A-Z]/\l&/')API.h"; \
			if [ -f "$$api_file" ]; then \
				echo "  Removing generated API header: $$api_file"; \
				rm -f "$$api_file"; \
			fi; \
		fi; \
	done

clean-all: clean clean-api-headers
	@echo "===> Full clean (root build + modules + generated API headers)"
	$(SILENT_PREFIX)for dir in $(SUBDIRS); do \
	    if [ -d "$$dir" ] && [ -f "$$dir/Makefile" ]; then \
	        echo "Cleaning module: $$dir"; \
	        $(MAKE) -C "$$dir" clean VERBOSE=$(VERBOSE); \
	    fi; \
	done

rebuild: clean-all all

rebuild-libs: clean-libs libs

rebuild-bin: clean-bin bin

rebuild-tests: clean tests

# ───────────────────────────────────────────────────────────────
# Documentation & Help
# ───────────────────────────────────────────────────────────────

docs:
	@./generate-root-docs.sh

help:
	@echo "Usage: make [TARGET] [OPTIONS]"
	@echo ""
	@echo "Main targets:"
	@echo "  all             = libs + bin"
	@echo "  rebuild         = clean-all + all"
	@echo "  libs            = build static libraries (incremental)"
	@echo "  bin             = build lobby executable (needs libs)"
	@echo "  run-client      = build & run lobby/$(MAIN_NAME)"
	@echo "  run-server      = build libs & run reseau server"
	@echo ""
	@echo "Testing:"
	@echo "  tests           = build all test binaries"
	@echo "  run-tests       = build + run all tests"
	@echo "  rebuild-tests   = clean + tests"
	@echo ""
	@echo "Cleaning:"
	@echo "  clean           = remove root build/ only"
	@echo "  clean-libs      = clean libraries (root + modules)"
	@echo "  clean-bin       = remove bin/ only"
	@echo "  clean-all       = full clean (build/ + modules + API headers)"
	@echo ""
	@echo "Documentation:"
	@echo "  docs            = generate root documentation"
	@echo "  help            = show this message"
	@echo ""
	@echo "Options:"
	@echo "  MODE=release|debug|strict-debug|...    (default: release)"
	@echo "  VERBOSE=1                              (default: 0 = silent)"
	@echo "  MAIN_NAME=xxx                          (default: main)"
	@echo ""
	@echo "Notes:"
	@echo "  • Libraries are built lazily (only when sources change)"
	@echo "  • clean-all removes API headers from firstparty/APIs/"
	@echo "  • Module sub-makefiles are expected to understand: clean, tests, run-tests, static-lib"

.PHONY: all libs bin run-client run-server
.PHONY: tests run-tests rebuild rebuild-libs rebuild-bin rebuild-tests
.PHONY: clean clean-libs clean-bin clean-all clean-api-headers
.PHONY: docs help