# ───────────────────────────────────────────────────────────────
# Root Makefile ─ Project build orchestration
# ───────────────────────────────────────────────────────────────

# Project info
NAME          := Multi-Mini-Games
MAIN_NAME     := main

# Dirs
BIN_DIR       := ./build/bin
LIB_DIR       := ./build/lib
OBJ_DIR       := ./build/obj
API_DIR       := ./firstparty/APIs

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

# Verbose control
ifeq ($(VERBOSE),1)
    SILENT_PREFIX :=
else
    SILENT_PREFIX := @
endif

ROOT_DIR      := .
MAKEFLAGS     += --no-print-directory

# Directories to exclude when discovering modules
EXCLUDED_DIRS := \
    assets       \
    build        \
    docs         \
    logs         \
    thirdparty   \
    tui-ver

# Discover module directories (exclude listed ones)
MODULES := $(patsubst %/,%,$(wildcard */))
MODULES := $(filter-out $(EXCLUDED_DIRS),$(MODULES))

SUBDIRS       := $(MODULES)

# Library outputs from modules (needed for final link)
# Order: firstparty and games first, then lobby, then reseau
ORDERED_MODULES := firstparty bingo chess king-for-four rubik lobby reseau
LIBS_REL        := $(foreach mod,$(ORDERED_MODULES),$(LIB_DIR)/lib$(subst -,,$(mod)).a)

# Modes: release, debug, strict-debug, clang-debug, valgrind-debug
MODE ?= release

# ── Targets ──────────────────────────────────────────────────────────────────

all: bin server

# Ensure directories exist
$(BIN_DIR) $(LIB_DIR) $(OBJ_DIR) $(API_DIR):
	$(SILENT_PREFIX)mkdir -p $@

# Module build rule
module-%: | $(LIB_DIR) $(API_DIR)
	@echo "Building static library -> $* (lib$*.a)"
	$(SILENT_PREFIX)$(MAKE) -j1 -C $* static-lib MODE=$(MODE) VERBOSE=$(VERBOSE) LIB_NAME=$(subst -,,$*) EXTRA_CFLAGS="-I$(CURDIR)/firstparty -I$(CURDIR)/thirdparty"
	$(SILENT_PREFIX)if cmp -s "$*/build/lib/lib$(subst -,,$*).a" "$(LIB_DIR)/lib$(subst -,,$*).a" 2>/dev/null; then \
		echo "  lib$(subst -,,$*).a unchanged"; \
	else \
		echo "  Updating lib$(subst -,,$*).a"; \
		cp "$*/build/lib/lib$(subst -,,$*).a" "$(LIB_DIR)/lib$(subst -,,$*).a"; \
	fi
	$(SILENT_PREFIX)mkdir -p $(API_DIR)
	$(SILENT_PREFIX)if [ -f "$*/include/$*API.h" ]; then \
		echo "  Updating API header -> $*API.h"; \
		cp "$*/include/$*API.h" "$(API_DIR)/"; \
	elif [ -f "$*/include/$(shell echo $* | sed -E 's/(-[a-z])/\U\1/g' | sed 's/-//g')API.h" ]; then \
		api_h="$(shell echo $* | sed -E 's/(-[a-z])/\U\1/g' | sed 's/-//g')API.h"; \
		echo "  Updating API header -> $$api_h"; \
		cp "$*/include/$$api_h" "$(API_DIR)/"; \
	else \
		echo "  Warning: API header not found for $*"; \
	fi

# Specific module targets
modules: $(foreach mod,$(ORDERED_MODULES),module-$(mod))

# Build main lobby binary
bin: modules | $(BIN_DIR)
	@echo "Building lobby executable..."
	$(SILENT_PREFIX)$(MAKE) -C lobby MODE=$(MODE) VERBOSE=$(VERBOSE) EXTRA_LDFLAGS="-L../build/lib -Wl,--start-group -lreseau -lbingo -lkingforfour -lchess -lrubik -lfirstparty -Wl,--end-group -lm"
	$(SILENT_PREFIX)install -m 755 lobby/build/bin/main$(EXE_EXT) $(BIN_DIR)/main$(EXE_EXT)

# Build server binary
server: modules | $(BIN_DIR)
	@echo "Building server executable..."
	$(SILENT_PREFIX)$(MAKE) -C reseau MODE=$(MODE) VERBOSE=$(VERBOSE) EXTRA_LDFLAGS="-L../build/lib -Wl,--start-group -llobby -lbingo -lkingforfour -lchess -lrubik -lfirstparty -Wl,--end-group -lm"
	$(SILENT_PREFIX)install -m 755 reseau/build/bin/server$(EXE_EXT) $(BIN_DIR)/server$(EXE_EXT)

clean:
	$(SILENT_PREFIX)rm -rf $(BUILD_DIR)
	$(SILENT_PREFIX)for dir in $(MODULES); do \
		if [ -d "$$dir" ] && [ -f "$$dir/Makefile" ]; then \
			module_name=$$(basename "$$dir"); \
			api_file="$(API_DIR)/$$(echo "$$module_name" | sed -E 's/[-_]([a-z])/\U\1/g' | sed 's/^[A-Z]/\l&/')API.h"; \
			if [ -f "$$api_file" ]; then \
				rm -f "$$api_file"; \
			fi; \
		fi; \
	done
	@echo "===> Full clean (root build + modules + generated API headers)"
	$(SILENT_PREFIX)for dir in $(MODULES); do \
		if [ -d "$$dir" ] && [ -f "$$dir/Makefile" ]; then \
			echo "Cleaning module: $$dir"; \
			$(MAKE) -C "$$dir" clean VERBOSE=$(VERBOSE); \
		fi; \
	done

rebuild: clean all

run-client: bin
	$(SILENT_PREFIX)if [ -f "$(BIN_DIR)/$(MAIN_NAME)$(EXE_EXT)" ]; then \
		echo "===> Starting client..."; \
		$(BIN_DIR)/$(MAIN_NAME)$(EXE_EXT); \
	else \
		echo "No executable found: $(BIN_DIR)/$(MAIN_NAME)$(EXE_EXT)"; \
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

help:
	@echo "Multi-Mini-Games root Makefile"
	@echo "Usage: make [target] [MODE=mode] [VERBOSE=1]"
	@echo ""
	@echo "Targets:"
	@echo "  all (default)  Build modules, lobby client and server"
	@echo "  modules        Build all static libraries in subdirectories"
	@echo "  bin            Build the main lobby executable"
	@echo "  server         Build the game server executable"
	@echo "  clean          Remove all build artifacts"
	@echo "  rebuild        Clean and build everything"
	@echo "  run-client     Run the lobby client"
	@echo "  run-server     Run the game server"
	@echo ""
	@echo "Modes:"
	@echo "  release (def)  Optimized build"
	@echo "  debug          Standard debug info"
	@echo "  clang-debug    Debug with AddressSanitizer/UBSan (requires clang)"
	@echo "  valgrind-debug Debug for memory leak checks"

.PHONY: all modules bin server clean rebuild run-client run-server help
