# ───────────────────────────────────────────────────────────────
# Suika Game Makefile
# Build: make [MODE=release|debug|strict-debug|clang-debug|valgrind-debug]
# ───────────────────────────────────────────────────────────────

MODE ?= debug
VERBOSE ?= 0
MAIN_NAME := suika
LIB_NAME := suika

SILENT := $(if $(filter 1,$(VERBOSE)),,@)

CC := gcc
AR := ar

CFLAGS_common := -Iinclude -I../thirdparty -I../firstparty
LDFLAGS_common := -L../thirdparty/libs/raylib-5.5_linux_amd64 -l:libraylib.a -lm

ifeq ($(MODE),release)
    CFLAGS := -O2 -DNDEBUG $(CFLAGS_common)
    LDFLAGS := -O2 $(LDFLAGS_common)
else ifeq ($(MODE),debug)
    CFLAGS := -Wall -Wextra -g -O0 -D_DEBUG $(CFLAGS_common)
    LDFLAGS := -g -rdynamic -O0 $(LDFLAGS_common)
else ifeq ($(MODE),strict-debug)
    CFLAGS := -Werror -Wall -Wextra -pedantic -g -O0 -D_DEBUG $(CFLAGS_common)
    LDFLAGS := -g -rdynamic -O0 -fno-omit-frame-pointer $(LDFLAGS_common)
else ifeq ($(MODE),clang-debug)
    CC := clang
    CFLAGS := -Werror -Wall -Wextra -pedantic -g -O0 -D_DEBUG -fsanitize=address,undefined -Wno-gnu-zero-variadic-macro-arguments $(CFLAGS_common)
    LDFLAGS := -g -O0 -fsanitize=address,undefined $(LDFLAGS_common)
else ifeq ($(MODE),valgrind-debug)
    CFLAGS := -Werror -Wall -Wextra -pedantic -g -O0 -D_DEBUG $(CFLAGS_common)
    LDFLAGS := -g -rdynamic -O0 -fno-omit-frame-pointer $(LDFLAGS_common)
else
    $(error Unknown MODE=$(MODE). Use: release, debug, strict-debug, clang-debug, valgrind-debug)
endif

SRC_DIR := src
BUILD_DIR := build
OBJ_DIR := $(BUILD_DIR)/obj
LIB_DIR := $(BUILD_DIR)/lib
BIN_DIR := $(BUILD_DIR)/bin

LIB_SOURCES := $(shell find $(SRC_DIR) -name '*.c' ! -name 'main.c')
MAIN_SOURCE := $(SRC_DIR)/main.c
LIB_OBJECTS := $(LIB_SOURCES:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)
MAIN_OBJECT := $(OBJ_DIR)/main.o
DEPS := $(LIB_OBJECTS:.o=.d) $(MAIN_OBJECT:.o=.d)

BIN := $(BIN_DIR)/$(MAIN_NAME)
STATIC_LIB := $(LIB_DIR)/lib$(LIB_NAME).a

CFLAGS += -MMD -MP
-include $(DEPS)

.PHONY: all clean rebuild run run-main run-gdb help

all: $(BIN) $(STATIC_LIB)

$(BIN): $(MAIN_OBJECT) $(STATIC_LIB)
	$(SILENT)mkdir -p $(@D)
	$(SILENT)$(CC) $(MAIN_OBJECT) -L$(LIB_DIR) -l$(LIB_NAME) $(LDFLAGS) -o $@
	@echo "Built: $@"

$(STATIC_LIB): $(LIB_OBJECTS)
	$(SILENT)mkdir -p $(@D)
	$(SILENT)$(AR) rcs $@ $^
	@echo "Built: $@"

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(SILENT)mkdir -p $(@D)
	$(SILENT)$(CC) $(CFLAGS) -c $< -o $@

clean:
	$(SILENT)rm -rf $(BUILD_DIR)

rebuild: clean all

run: all
	@./$(BIN)

run-main: all
ifeq ($(MODE),valgrind-debug)
	@mkdir -p logs
	@valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./$(BIN) 2>&1 | tee logs/valgrind-$$(date +%d-%m-%Y-%H-%M-%S).log
else
	@./$(BIN)
endif

run-gdb: all
	@gdb ./$(BIN)

help:
	@echo "Usage: make [TARGET] [MODE=...]"
	@echo ""
	@echo "Targets: all | clean | rebuild | run | run-main | run-gdb | help"
	@echo ""
	@echo "Modes:"
	@echo "  release        - Optimized build"
	@echo "  debug          - Debug build (default)"
	@echo "  strict-debug   - Strict debug with -Werror"
	@echo "  clang-debug    - Clang with sanitizers"
	@echo "  valgrind-debug - For valgrind testing"
	@echo ""
	@echo "Options: VERBOSE=1 to show commands"
