## @file    Makefile
## @brief   Système de build pour King For Four 

# ==========================================
# 1. CONFIGURATION DU PROJET
# ==========================================

# Nom de l'exécutable final
APP_NAME := king_for_four

# Nom du fichier contenant le main() 
MAIN_NAME := main

# Dossiers du projet
SRC_DIR   := src
TEST_DIR  := tests
BUILD_DIR := build
INC_DIR   := include

# Chemins vers Raylib
RAYLIB_PATH := ../thirdparty/raylib/raylib-5.5_linux_amd64
RAYLIB_INC  := $(RAYLIB_PATH)/include
RAYLIB_LIB  := $(RAYLIB_PATH)/lib

# ==========================================
# 2. COMPILATEUR ET FLAGS
# ==========================================

# Flags de compilation (C)
# -I$(INC_DIR) permet de faire #include "core/game.h" proprement
BASE_CFLAGS := \
    -std=c99 \
    -D_DEFAULT_SOURCE \
    -Wno-missing-braces \
    -Wno-unused-value \
    -Wno-pointer-sign \
    -I$(INC_DIR) \
    -I$(RAYLIB_INC)

# Flags de l'éditeur de liens (Linker)
BASE_LDFLAGS := \
    -L$(RAYLIB_LIB) \
    -l:libraylib.a \
    -lGL -lm -lpthread -ldl -lrt \
    -Xlinker -zmuldefs

# ==========================================
# 3. MODES DE COMPILATION
# ==========================================
MODE ?= release

ifeq ($(MODE),release)
    CC := gcc
    CFLAGS := -O2 -s -Wall
    LDFLAGS := -O2
else ifeq ($(MODE),debug)
    CC := gcc
    CFLAGS := -g -O0 -Wall -Wextra -D_DEBUG
    LDFLAGS := -g -O0 -rdynamic
else ifeq ($(MODE),valgrind-debug)
    CC := gcc
    CFLAGS := -g -O0 -Wall -Wextra -D_DEBUG
    LDFLAGS := -g -O0
    TOOL := valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes
endif

# Ajout des flags de base aux flags spécifiques du mode
CFLAGS  += $(BASE_CFLAGS)
LDFLAGS += $(BASE_LDFLAGS)

# ==========================================
# 4. DÉTECTION AUTOMATIQUE DES FICHIERS
# ==========================================

OBJ_DIR := $(BUILD_DIR)/obj
BIN_DIR := $(BUILD_DIR)/bin
BIN     := $(BIN_DIR)/$(APP_NAME)

# Trouve tous les fichiers .c dans src/
ALL_SRC := $(shell find $(SRC_DIR) -name '*.c')

# Isole le main.c
MAIN_SRC := $(shell find $(SRC_DIR) -name '$(MAIN_NAME).c')

# Tout le reste, ce sont les fichiers de logique 
LIB_SOURCES := $(filter-out $(MAIN_SRC), $(ALL_SRC))

# Génère les chemins des objets (.o) en miroir de src/
LIB_OBJECTS := $(LIB_SOURCES:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)
MAIN_OBJECT := $(MAIN_SRC:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

# Gestion des dépendances (.d) pour recompiler si un .h change
DEPS := $(LIB_OBJECTS:.o=.d) $(MAIN_OBJECT:.o=.d)
DEP_FLAGS := -MMD -MP

# ==========================================
# 5. RÈGLES DE BUILD
# ==========================================

.PHONY: all clean rebuild run dirs info

all: dirs $(BIN)

dirs:
	@mkdir -p $(BIN_DIR)
	@mkdir -p $(OBJ_DIR)

# Ligne de commande finale pour créer l'exécutable
$(BIN): $(LIB_OBJECTS) $(MAIN_OBJECT)
	@echo "LINKING: $@"
	@$(CC) $^ $(LDFLAGS) -o $@

# Règle magique pour compiler n'importe quel .c dans src/ vers build/obj/
# Le 'mkdir -p $(@D)' recrée la structure de dossiers (core, ui) automatiquement
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(@D)
	@echo "COMPILING: $<"
	@$(CC) $(CFLAGS) $(DEP_FLAGS) -c $< -o $@

-include $(DEPS)

# ==========================================
# 6. COMMANDES UTILES
# ==========================================

clean:
	@echo "CLEANING..."
	@rm -rf $(BUILD_DIR)

rebuild: clean all

run: all
	@echo "RUNNING $(APP_NAME)..."
	@if [ "$(MODE)" = "valgrind-debug" ]; then \
		$(TOOL) $(BIN); \
	else \
		$(BIN); \
	fi

# Pour vérifier si le Makefile trouve bien tes fichiers
info:
	@echo "Sources trouvées :"
	@echo "  Main    : $(MAIN_SRC)"
	@echo "  Core/UI : $(LIB_SOURCES)"
	@echo "  Objets  : $(LIB_OBJECTS)"