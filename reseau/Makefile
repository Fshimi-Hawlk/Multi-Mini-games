# Makefile pour le module RESEAU
# Compatible avec le build system racine

CC = gcc
# Includes complets
CFLAGS = -Wall -Wextra -Iinclude -I../firstparty -I../thirdparty -O2

# 1. Configuration de la Librairie (contient lobby.c et lobby_module)
LIB_NAME ?= reseau
LIB_SRC = src/lobby.c
LIB_OBJ = build/obj/lobby.o
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

# CORRECTION ICI : On ajoute $(LIB_OBJ) pour que le serveur connaisse 'lobby_module'
$(SERVER_BIN): $(SERVER_OBJ) $(LIB_OBJ)
	@mkdir -p build/bin
	$(CC) $(CFLAGS) $^ -o $@
	@echo "[RESEAU] Serveur compilé avec succès : $@"

# --- Compilation générique ---
build/obj/%.o: src/%.c
	@mkdir -p build/obj
	$(CC) $(CFLAGS) -c $< -o $@

# --- Nettoyage ---
clean:
	rm -rf build
	@echo "[RESEAU] Nettoyé."

.PHONY: all static-lib server clean
