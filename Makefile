# ───────────────────────────────────────────────────────────────
# Top-Level Makefile ─ Project build system
# ───────────────────────────────────────────────────────────────

# Force l'utilisation de sh pour la compatibilité
SHELL = /bin/sh

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
EXCLUDED_DIRS := assets build docs firstparty lobby logs thirdparty tui-ver

# Detect modules
MODULES := $(patsubst %/,%,$(wildcard */))
MODULES := $(filter-out $(EXCLUDED_DIRS), $(MODULES))

SUBDIRS := $(MODULES) lobby

FIRSTPARTY_API_DIR := firstparty/APIs

# Root-level directories
BUILD_DIR := build
LIB_DIR   := $(BUILD_DIR)/lib
BIN_DIR   := $(BUILD_DIR)/bin

# Computed lib names
define compute-lib-name
$(shell echo '$(1)' | tr '[:upper:]' '[:lower:]' | tr -d '_-')
endef

define compute-api-name
$(shell echo '$(1)API.h')
endef

LIBS := $(foreach mod,$(MODULES),$(LIB_DIR)/lib$(call compute-lib-name,$(mod)).a)
LIBS_REL := $(foreach lib,$(LIBS),../$(lib))

# ───────────────────────────────────────────────────────────────
# Main Targets
# ───────────────────────────────────────────────────────────────

# MODIFICATION ICI : On construit le jeu (bin) ET le serveur
all: bin server

# Nouvelle cible pour le serveur
server:
	$(SILENT_PREFIX)echo "Building dedicated server..."
	$(SILENT_PREFIX)$(MAKE) -C reseau server

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
		EXTRA_CFLAGS="-DASSET_PATH=\"$(MOD_DIR)/assets/\""
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

bin: libs
	$(SILENT_PREFIX)echo "Building lobby executable (if needed)..."
	$(SILENT_PREFIX)mkdir -p $(BIN_DIR)
	$(SILENT_PREFIX)$(MAKE) -C lobby \
		MODE=$(MODE) \
		VERBOSE=$(VERBOSE) \
		BIN_DIR=../$(BIN_DIR) \
		EXTRA_CFLAGS="-DASSET_PATH=\"lobby/assets/\" -I../reseau/include" \
		EXTRA_LDFLAGS="$(LIBS_REL)"

rebuild-exe: libs
	$(SILENT_PREFIX)echo "Force rebuilding lobby executable..."
	$(SILENT_PREFIX)rm -f $(BIN_DIR)/$(MAIN_NAME)
	$(SILENT_PREFIX)mkdir -p $(BIN_DIR)
	$(SILENT_PREFIX)$(MAKE) -C lobby \
		MODE=$(MODE) \
		VERBOSE=$(VERBOSE) \
		BIN_DIR=../$(BIN_DIR) \
		EXTRA_CFLAGS="-DASSET_PATH=\"lobby/assets/\" -I../reseau/include" \
		EXTRA_LDFLAGS="$(LIBS_REL)" \
		rebuild

run-exe:
	@if [ -f $(BIN_DIR)/$(MAIN_NAME) ]; then \
		$(BIN_DIR)/$(MAIN_NAME); \
	else \
		echo "No executable found at $(BIN_DIR)/$(MAIN_NAME)"; \
		echo "Run 'make bin' or 'make rebuild-exe' first."; \
	fi

# Tests section omitted for brevity but standard targets follow...
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

help:
	@echo "Usage: make [OPTIONS] [TARGET]"
	@echo "TARGETS: all, server, rebuild, clean, clean-all, libs, bin"
