# Makefile pour le module RESEAU
# Compatible avec le build system racine

# Includes complets
BASE_CFLAGS = -Wall -Wextra -Iinclude -I../firstparty -I../thirdparty

# Modes de compilation
MODE ?= release

ifeq ($(MODE),release)
    CC := gcc
    CFLAGS := -O2
    LDFLAGS := -O2
else ifeq ($(MODE),debug)
    CC := gcc
    CFLAGS := -g -O0 -D_DEBUG
    LDFLAGS := -g -O0 -rdynamic
else ifeq ($(MODE),clang-debug)
    CC := clang
    CFLAGS := -g -O0 -fsanitize=address,undefined -fno-omit-frame-pointer -D_DEBUG
    LDFLAGS := -g -O0 -fsanitize=address,undefined
else ifeq ($(MODE),valgrind-debug)
    CC := gcc
    CFLAGS := -g -O0 -D_DEBUG
    LDFLAGS := -g -O0
endif

CFLAGS += $(BASE_CFLAGS)

# 1. Configuration de la Librairie
LIB_NAME ?= reseau
LIB_SRC = src/lobby.c src/rudp_core.c
LIB_OBJ = build/obj/lobby.o build/obj/rudp_core.o
LIB_TARGET = build/lib/lib$(LIB_NAME).a

# 2. Configuration du Serveur
SERVER_SRC = src/server.c
SERVER_OBJ = build/obj/server.o
SERVER_BIN = build/bin/server

# Cibles principales
all: static-lib server

# --- Librairie Statique ---
static-lib: $(LIB_TARGET)

$(LIB_TARGET): $(LIB_OBJ)
	@mkdir -p build/lib
	ar rcs $@ $^
	@echo "[RESEAU] Librairie statique générée : $@"

# --- Serveur ---
server: $(SERVER_BIN)

$(SERVER_BIN): $(SERVER_OBJ) $(LIB_OBJ)
	@mkdir -p build/bin
	$(CC) $(CFLAGS) $(EXTRA_CFLAGS) $^ $(EXTRA_LDFLAGS) $(LDFLAGS) -o $@
	@echo "[RESEAU] Serveur compilé avec succès : $@"

# --- Compilation générique ---
build/obj/%.o: src/%.c
	@mkdir -p build/obj
	$(CC) $(CFLAGS) $(EXTRA_CFLAGS) -c $< -o $@

# --- Nettoyage ---
clean:
	rm -rf build
	@echo "[RESEAU] Nettoyé."

.PHONY: all static-lib server clean
