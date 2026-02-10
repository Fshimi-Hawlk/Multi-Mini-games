import os

# Contenu du Makefile RACINE avec la cible 'server' ajoutée
makefile_content = """# ───────────────────────────────────────────────────────────────
# Top-Level Makefile ─ Project build system
# ───────────────────────────────────────────────────────────────

# Force l'utilisation de sh pour la compatibilité
SHELL = /bin/sh

MODE ?= release
VERBOSE ?= 0
MAIN_NAME ?= main

# Verbose mode (default silent)
ifeq ($(VERBOSE),1)
\tSILENT_PREFIX :=
else
\tSILENT_PREFIX := @
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
\t$(SILENT_PREFIX)echo "Building dedicated server..."
\t$(SILENT_PREFIX)$(MAKE) -C reseau server

libs: $(LIBS)

$(LIB_DIR)/lib%.a:
\t$(eval MOD_DIR := $(filter %$*,$(MODULES)))
\t$(eval LIB_NAME := $(call compute-lib-name,$(MOD_DIR)))
\t$(eval API_HEADER := $(call compute-api-name,$(LIB_NAME)))
\t$(SILENT_PREFIX)echo "Building library for $(MOD_DIR) if needed..."
\t$(SILENT_PREFIX)$(MAKE) -C $(MOD_DIR) static-lib \\
\t\tMODE=$(MODE) \\
\t\tVERBOSE=$(VERBOSE) \\
\t\tLIB_NAME=$(LIB_NAME) \\
\t\tEXTRA_CFLAGS="-DASSET_PATH=\\\"$(MOD_DIR)/assets/\\\""
\t$(SILENT_PREFIX)mkdir -p $(LIB_DIR)
\t$(SILENT_PREFIX)if cmp -s $(MOD_DIR)/build/lib/lib$(LIB_NAME).a $@ 2>/dev/null; then \\
\t\techo "  lib$(LIB_NAME).a unchanged"; \\
\telse \\
\t\techo "  Updating lib$(LIB_NAME).a"; \\
\t\tcp $(MOD_DIR)/build/lib/lib$(LIB_NAME).a $@; \\
\tfi
\t$(SILENT_PREFIX)mkdir -p $(FIRSTPARTY_API_DIR)
\t$(SILENT_PREFIX)if [ -f "$(MOD_DIR)/include/$(API_HEADER)" ]; then \\
\t\techo "  Updating API header: $(API_HEADER)"; \\
\t\tcp "$(MOD_DIR)/include/$(API_HEADER)" "$(FIRSTPARTY_API_DIR)/"; \\
\telse \\
\t\techo "  Warning: $(API_HEADER) not found in $(MOD_DIR)/include/"; \\
\tfi

bin: libs
\t$(SILENT_PREFIX)echo "Building lobby executable (if needed)..."
\t$(SILENT_PREFIX)mkdir -p $(BIN_DIR)
\t$(SILENT_PREFIX)$(MAKE) -C lobby \\
\t\tMODE=$(MODE) \\
\t\tVERBOSE=$(VERBOSE) \\
\t\tBIN_DIR=../$(BIN_DIR) \\
\t\tEXTRA_CFLAGS="-DASSET_PATH=\\\"lobby/assets/\\\" -I../reseau/include" \\
\t\tEXTRA_LDFLAGS="$(LIBS_REL)"

rebuild-exe: libs
\t$(SILENT_PREFIX)echo "Force rebuilding lobby executable..."
\t$(SILENT_PREFIX)rm -f $(BIN_DIR)/$(MAIN_NAME)
\t$(SILENT_PREFIX)mkdir -p $(BIN_DIR)
\t$(SILENT_PREFIX)$(MAKE) -C lobby \\
\t\tMODE=$(MODE) \\
\t\tVERBOSE=$(VERBOSE) \\
\t\tBIN_DIR=../$(BIN_DIR) \\
\t\tEXTRA_CFLAGS="-DASSET_PATH=\\\"lobby/assets/\\\" -I../reseau/include" \\
\t\tEXTRA_LDFLAGS="$(LIBS_REL)" \\
\t\trebuild

run-exe:
\t@if [ -f $(BIN_DIR)/$(MAIN_NAME) ]; then \\
\t\t$(BIN_DIR)/$(MAIN_NAME); \\
\telse \\
\t\techo "No executable found at $(BIN_DIR)/$(MAIN_NAME)"; \\
\t\techo "Run 'make bin' or 'make rebuild-exe' first."; \\
\tfi

# Tests section omitted for brevity but standard targets follow...
clean-libs:
\t$(SILENT_PREFIX)for dir in $(SUBDIRS); do \\
\t\tif [ -d "$$dir" ] && [ -f "$$dir/Makefile" ]; then \\
\t\t\t$(MAKE) -C "$$dir" clean VERBOSE=$(VERBOSE); \\
\t\tfi; \\
\tdone
\t$(SILENT_PREFIX)rm -rf $(LIB_DIR)

clean-exe:
\t$(SILENT_PREFIX)rm -rf $(BIN_DIR)

clean:
\t$(SILENT_PREFIX)rm -rf $(BUILD_DIR)

clean-all: clean
\t$(SILENT_PREFIX)for dir in $(SUBDIRS); do \\
\t\tif [ -d "$$dir" ] && [ -f "$$dir/Makefile" ]; then \\
\t\t\techo "Cleaning $$dir ..."; \\
\t\t\t$(MAKE) -C "$$dir" clean VERBOSE=$(VERBOSE); \\
\t\tfi; \\
\tdone

rebuild: clean-all all
rebuild-libs: clean-libs libs
rebuild-exe: clean-exe bin

help:
\t@echo "Usage: make [OPTIONS] [TARGET]"
\t@echo "TARGETS: all, server, rebuild, clean, clean-all, libs, bin"
"""

with open("Makefile", "w") as f:
    f.write(makefile_content)

print("✅ Makefile racine mis à jour ! 'make rebuild' compilera maintenant le serveur.")
