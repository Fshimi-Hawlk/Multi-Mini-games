## @file    Makefile
## @brief   Système de build hybride (API + Standalone) pour King For Four 

# ==========================================
# 1. CONFIGURATION DU PROJET
# ==========================================

# Noms des cibles
APP_NAME := king_for_four
MAIN_NAME := main

# Dossiers du projet
SRC_DIR   := src
TEST_DIR  := tests
BUILD_DIR := build
INC_DIR   := include

# --- AJOUT MONOREPO ---
LIB_DIR    := $(BUILD_DIR)/lib
STATIC_LIB := $(LIB_DIR)/libkingforfour.a
FIRSTPARTY := ../firstparty

# Chemins vers Raylib (Adapté à l'arborescence du monorepo)
RAYLIB_PATH := ../thirdparty/libs/raylib-5.5_linux_amd64
RAYLIB_INC  := ../thirdparty
RAYLIB_LIB  := ../thirdparty/libs/raylib-5.5_linux_amd64

# ==========================================
# 2. COMPILATEUR ET FLAGS
# ==========================================

# Flags de compilation (C)
BASE_CFLAGS := \
	-std=c99 \
	-D_DEFAULT_SOURCE \
	-Wno-missing-braces \
	-Wno-unused-value \
	-Wno-pointer-sign \
	-I$(INC_DIR) \
	-I$(FIRSTPARTY) \
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
    VG_FLAGS := --leak-check=full --show-leak-kinds=all --track-origins=yes
    ifeq ($(VG_COMPAT),1)
        $(info Valgrind: Compatibility mode enabled (--smc-check=all-non-file))
        VG_FLAGS += --smc-check=all-non-file
    endif
    TOOL := valgrind $(VG_FLAGS)
else ifeq ($(MODE),clang-debug)
    CC := clang
    CFLAGS := -g -O0 -Wall -Wextra -D_DEBUG -fsanitize=address,undefined -fno-omit-frame-pointer
    LDFLAGS := -g -O0 -fsanitize=address,undefined
endif

CFLAGS  += $(BASE_CFLAGS) $(EXTRA_CFLAGS)
LDFLAGS += $(BASE_LDFLAGS) $(EXTRA_LDFLAGS)

# ==========================================
# 4. DÉTECTION AUTOMATIQUE DES FICHIERS
# ==========================================

OBJ_DIR := $(BUILD_DIR)/obj
BIN_DIR := $(BUILD_DIR)/bin
BIN     := $(BIN_DIR)/$(APP_NAME)

# Trouve tous les fichiers .c dans src/
ALL_SRC := $(shell find $(SRC_DIR) -name '*.c')

# Isole le main.c (Ignoré par la compilation de la librairie)
MAIN_SRC := $(shell find $(SRC_DIR) -name '$(MAIN_NAME).c')

# Tout le reste, ce sont les fichiers de logique (incluant kingforfourAPI.c)
LIB_SOURCES := $(filter-out $(MAIN_SRC), $(ALL_SRC))

# Génère les chemins des objets (.o) en miroir de src/
LIB_OBJECTS := $(LIB_SOURCES:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)
MAIN_OBJECT := $(MAIN_SRC:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

DEPS := $(LIB_OBJECTS:.o=.d) $(MAIN_OBJECT:.o=.d)
DEP_FLAGS := -MMD -MP

# ==========================================
# 5. RÈGLES DE BUILD
# ==========================================

.PHONY: all clean rebuild run-main static-lib dirs info

all: dirs static-lib $(BIN)

dirs:
	@mkdir -p $(BIN_DIR)
	@mkdir -p $(OBJ_DIR)
	@mkdir -p $(LIB_DIR)

# --- CIBLE REQUISE PAR LE LOBBY (API CONVERSION) ---
static-lib: dirs $(STATIC_LIB)

$(STATIC_LIB): $(LIB_OBJECTS)
	@echo "ARCHIVING: $@"
	@ar rcs $@ $^

# Ligne de commande finale pour créer l'exécutable standalone
$(BIN): $(STATIC_LIB) $(MAIN_OBJECT)
	@echo "LINKING STANDALONE: $@"
	@$(CC) $(MAIN_OBJECT) $(STATIC_LIB) $(LDFLAGS) -o $@

# Règle magique pour compiler n'importe quel .c dans src/ vers build/obj/
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

# --- COMMANDE MODIFIÉE SELON API_CONVERSION.MD ---
run-main: all
	@echo "RUNNING $(APP_NAME)..."
	@if [ "$(MODE)" = "valgrind-debug" ]; then \
	    $(TOOL) $(BIN); \
	else \
	    $(BIN); \
	fi

info:
	@echo "Sources trouvées :"
	@echo "  Main    : $(MAIN_SRC)"
	@echo "  Core/UI : $(LIB_SOURCES)"
	@echo "  Objets  : $(LIB_OBJECTS)"
	@echo "  Lib     : $(STATIC_LIB)"