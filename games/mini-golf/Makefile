## @file    Makefile
## @brief   Makefile for the Golf 3D mini-game
##
## @details Supported compilation modes:
##            - release         : optimized build using GCC
##            - debug           : warnings + debug symbols using GCC
##            - strict-debug    : pedantic checks using GCC
##            - clang-debug     : sanitizers + pedantic checks using Clang
##            - valgrind-debug  : debug build using GCC, runs under Valgrind
##
## @section Key targets
##            - all              : build main executable (default)
##            - static-lib       : build static library libgolf.a
##            - rebuild          : clean and rebuild main

# ============================================================
# Platform Detection
# ============================================================
UNAME_S := $(shell uname -s)
UNAME_R := $(shell uname -r)

ifeq ($(findstring Darwin,$(UNAME_S)),Darwin)
    OS := darwin
else ifeq ($(findstring Linux,$(UNAME_S)),Linux)
    ifneq ($(findstring microsoft,$(UNAME_R)),)
        OS := wsl
    else
        OS := linux
    endif
else ifeq ($(findstring MINGW,$(UNAME_S)),MINGW)
    OS := mingw
else ifeq ($(findstring MSYS,$(UNAME_S)),MSYS)
    OS := mingw
else
    $(error Unsupported OS: $(UNAME_S))
endif

include make/platform/$(OS).mk

ifeq ($(OS),mingw)
    EXE_EXT := .exe
else
    EXE_EXT :=
endif

# ============================================================
# Compiler / Mode
# ============================================================
MODE ?= release

ifeq ($(MODE),release)
	CC      := gcc
	CFLAGS  := -O2
	LDFLAGS := -O2
	TOOL    :=
else ifeq ($(MODE),debug)
	CC      := gcc
	CFLAGS  := -Wall -Wextra -g -O0 -Wno-unused-function -D_DEBUG
	LDFLAGS := -g -rdynamic -O0
	TOOL    :=
else ifeq ($(MODE),strict-debug)
	CC      := gcc
	CFLAGS  := -Werror -Wall -Wextra -pedantic -g -O0 \
	           -Wno-unused-function -D_DEBUG
	LDFLAGS := -g -rdynamic -O0 -fno-omit-frame-pointer
	TOOL    :=
else ifeq ($(MODE),clang-debug)
	CC      := clang
	CFLAGS  := -Werror -Wall -Wextra -pedantic -g -O0 \
	           -fno-omit-frame-pointer -fsanitize=address,undefined \
	           -Wno-unused-function -D_DEBUG
	LDFLAGS := -g -O0 -fno-omit-frame-pointer -fsanitize=address,undefined
	TOOL    :=
else ifeq ($(MODE),valgrind-debug)
	CC      := gcc
	CFLAGS  := -Wall -Wextra -g -O0 -Wno-unused-function -D_DEBUG
	LDFLAGS := -g -rdynamic -O0
	TOOL    := valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes
else
	$(error Unknown MODE=$(MODE). Use release, debug, strict-debug, clang-debug, valgrind-debug)
endif

CFLAGS  += $(BASE_CFLAGS)  $(EXTRA_CFLAGS)
LDFLAGS += $(BASE_LDFLAGS) $(EXTRA_LDFLAGS)

# ============================================================
# Paths
# ============================================================
MAIN_NAME   ?= main
SRC_DIR     := src
OBJ_DIR     := build/obj
BIN_DIR     := build/bin
LIB_DIR     := build/lib
BIN         := $(BIN_DIR)/$(MAIN_NAME)$(EXE_EXT)
STATIC_LIB  ?= $(LIB_DIR)/lib$(LIB_NAME).a

# ============================================================
# Sources
# ============================================================
# Library sources: everything EXCEPT golf_main.c (which has standalone main)
LIB_SOURCES := $(SRC_DIR)/golf_data.c  \
               $(SRC_DIR)/golf_course.c \
               $(SRC_DIR)/golf_ball.c   \
               $(SRC_DIR)/golf_camera.c \
               $(SRC_DIR)/golf_ui.c     \
               $(SRC_DIR)/golf_main.c   \
               $(SRC_DIR)/golfAPI.c    \
               $(SRC_DIR)/network/clientInterface.c

LIB_OBJECTS := $(LIB_SOURCES:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

# Standalone main (only for direct execution, not included in the library)
MAIN_SOURCE := $(SRC_DIR)/golf_standalone.c
MAIN_OBJECT := $(OBJ_DIR)/golf_standalone.o

VERBOSE ?= 0
ifeq ($(VERBOSE),1)
	SILENT_PREFIX :=
else
	SILENT_PREFIX := @
endif

DEP_FLAGS := -MMD -MP
DEPS      := $(LIB_OBJECTS:.o=.d) $(MAIN_OBJECT:.o=.d)
-include $(DEPS)

.SECONDARY: $(LIB_OBJECTS) $(MAIN_OBJECT)

# ============================================================
# Rules
# ============================================================
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(SILENT_PREFIX)mkdir -p $(@D)
	$(SILENT_PREFIX)$(CC) $(CFLAGS) $(DEP_FLAGS) -c $< -o $@

# ============================================================
# Targets
# ============================================================
.PHONY: all static-lib rebuild rebuild-obj clean run-main run-gdb help

all: $(BIN)

$(BIN): $(LIB_OBJECTS) $(MAIN_OBJECT)
	$(SILENT_PREFIX)mkdir -p $(@D)
	$(SILENT_PREFIX)$(CC) $^ $(LDFLAGS) -o $@

# Incremental static lib — no rebuild-obj dependency (fix)
static-lib: $(LIB_OBJECTS)
	$(SILENT_PREFIX)mkdir -p $(LIB_DIR)
	$(SILENT_PREFIX)ar rcs $(STATIC_LIB) $^

rebuild-obj:
	$(SILENT_PREFIX)rm -rf $(OBJ_DIR) $(LIB_DIR)
	$(SILENT_PREFIX)$(MAKE) $(LIB_OBJECTS)

rebuild: clean all

clean:
	$(SILENT_PREFIX)rm -rf $(OBJ_DIR) $(BIN_DIR) $(LIB_DIR)

run-main:
	@if [ -f $(BIN) ]; then $(TOOL) ./$(BIN); else echo "No executable"; fi

run-gdb:
	@if [ -f $(BIN) ]; then gdb $(BIN); else echo "No executable"; fi

help:
	@echo "Usage: make [TARGET] [OPTIONS]"
	@echo ""
	@echo "TARGETS:"
	@echo "    all          Build standalone executable"
	@echo "    static-lib   Build static library libgolf.a (used by lobby)"
	@echo "    rebuild      Force full recompilation"
	@echo "    clean        Remove build artifacts"
	@echo "    run-main     Run the standalone executable"
	@echo ""
	@echo "OPTIONS:"
	@echo "    MODE=<str>   release | debug | strict-debug | clang-debug | valgrind-debug"
	@echo "    VERBOSE=1    Show all commands"
