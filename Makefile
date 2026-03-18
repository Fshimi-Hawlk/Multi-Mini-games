# ───────────────────────────────────────────────────────────────
# Master Makefile ─ Système Monorepo Sécurisé pour Multi-Mini-Games
# ───────────────────────────────────────────────────────────────

SHELL = /bin/sh
MODE ?= release
VERBOSE ?= 0

# Fait Physique : Fige le chemin absolu de la racine du projet
# Garantit que les sous-Makefiles (lobby, reseau) trouvent toujours les API
ROOT_DIR := $(CURDIR)

# CONFIGURATION
MODULES_DIR := king-for-four
LIB_NAME_RAW := kingforfour

ifeq ($(VERBOSE),1)
    SILENT := 
else
    SILENT := @
endif

# Utilisation stricte de chemins absolus pour éviter les désynchronisations de variables
BUILD_DIR := $(ROOT_DIR)/build
LIB_DIR   := $(BUILD_DIR)/lib
BIN_DIR   := $(BUILD_DIR)/bin
API_DIR   := $(ROOT_DIR)/firstparty/APIs

.PHONY: all prepare libs server bin run-server run-lobby clean rebuild

# Ordre d'exécution mathématique : L'API -> La Lib de jeu -> Le Serveur (RUDP) -> Le Client (Lobby)
all: prepare libs server bin

# =================================================================================
# 1. PRÉPARATION : Génération de l'API commune
# =================================================================================
prepare:
	$(SILENT)mkdir -p $(API_DIR) $(LIB_DIR) $(BIN_DIR)
	$(SILENT)echo "===> Preparing API headers with correct types..."
	$(SILENT)printf "#ifndef GENERAL_API_H\n#define GENERAL_API_H\n\ntypedef enum { OK = 0, ERROR_NULL_POINTER, ERROR_ALLOC, ERROR_INVALID } Error_Et;\n\ntypedef struct BaseGame_St {\n    int running;\n    int paused;\n    long score;\n    Error_Et (*freeGame)(void*);\n} BaseGame_St;\n\n#endif\n" > $(API_DIR)/generalAPI.h

# =================================================================================
# 2. LÉGION : Compilation du module de jeu en librairie statique
# =================================================================================
libs: prepare
	$(SILENT)echo "===> Building module: $(MODULES_DIR)"
	$(SILENT)$(MAKE) -C $(MODULES_DIR) static-lib \
		LIB_NAME=$(LIB_NAME_RAW) \
		MODE=$(MODE) \
		VERBOSE=$(VERBOSE) \
		EXTRA_CFLAGS="-I$(ROOT_DIR)/firstparty -I$(ROOT_DIR) -I$(ROOT_DIR)/reseau/include -I$(ROOT_DIR)/lobby/include"
	$(SILENT)cp $(ROOT_DIR)/$(MODULES_DIR)/build/lib/lib$(LIB_NAME_RAW).a $(LIB_DIR)/
	$(SILENT)if [ -f $(ROOT_DIR)/$(MODULES_DIR)/include/$(LIB_NAME_RAW)API.h ]; then \
		cp $(ROOT_DIR)/$(MODULES_DIR)/include/$(LIB_NAME_RAW)API.h $(API_DIR)/; \
	fi

# =================================================================================
# 3. RÉSEAU : Construction du serveur autoritaire (Backend)
# =================================================================================
server: libs
	$(SILENT)echo "===> Building Server"
	$(SILENT)$(MAKE) -C reseau all \
		MODE=$(MODE) \
		VERBOSE=$(VERBOSE) \
		EXTRA_CFLAGS="-I$(ROOT_DIR)/$(MODULES_DIR)/include -I$(ROOT_DIR) -I$(ROOT_DIR)/firstparty" \
		EXTRA_LDFLAGS="$(LIB_DIR)/lib$(LIB_NAME_RAW).a"

# =================================================================================
# 4. LOBBY : Construction du client (Interface Utilisateur / Frontend)
# Fait Physique : On force la dépendance à "server" pour garantir la création de libreseau.a
# =================================================================================
bin: libs server
	$(SILENT)echo "===> Building Lobby"
	$(SILENT)$(MAKE) -C lobby all \
		MODE=$(MODE) \
		VERBOSE=$(VERBOSE) \
		EXTRA_CFLAGS="-I$(ROOT_DIR)/reseau/include -I$(ROOT_DIR) -I$(ROOT_DIR)/firstparty" \
		EXTRA_LDFLAGS="$(LIB_DIR)/lib$(LIB_NAME_RAW).a $(ROOT_DIR)/reseau/build/lib/libreseau.a"

# =================================================================================
# 5. EXÉCUTION : Commandes de lancement isolées
# =================================================================================
run-server: server
	$(SILENT)echo "===> Starting Server..."
	$(SILENT)cd reseau && ./build/bin/server

run-lobby: bin
	$(SILENT)echo "===> Starting Lobby..."
	$(SILENT)cd lobby && ./build/bin/main

# =================================================================================
# 6. MAINTENANCE (Sécurisée)
# =================================================================================
clean:
	$(SILENT)echo "===> Cleaning Architecture..."
	$(SILENT)rm -rf $(BUILD_DIR)
	$(SILENT)rm -f $(API_DIR)/generalAPI.h $(API_DIR)/$(LIB_NAME_RAW)API.h
	$(SILENT)$(MAKE) -C $(MODULES_DIR) clean
	$(SILENT)$(MAKE) -C lobby clean
	$(SILENT)$(MAKE) -C reseau clean

rebuild: clean all