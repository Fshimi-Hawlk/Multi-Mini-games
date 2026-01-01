# Compiler and flags
# Base flags (always present)
BASE_CFLAGS := \
	-Wno-newline-eof \
	-Iinclude \
	-I../thirdparty \
	-I../firstparty \

# Linker base
BASE_LDFLAGS := \
	-L../thirdparty/libs/raylib-5.5_linux_amd64 \
	-l:libraylib.a \
	-lm

# Modes
MODE ?= release
ifeq ($(MODE),release)
    CC := gcc
    CFLAGS := \
		-O2
    LDFLAGS := \
		-O2
else ifeq ($(MODE),debug)
    CC := gcc
    CFLAGS := \
		-Wall \
		-Wextra \
		-g \
		-O0 \
		-Wno-unused-function \
		-Wno-deprecated-declarations \
		-D_STACK_TRACE \
		-D_DEBUG
    LDFLAGS := \
		-g \
		-rdynamic \
		-O0
else ifeq ($(MODE),extra-debug)
    CC := clang
    CFLAGS := \
		-Werror \
		-Wall \
		-Wextra \
		-pedantic \
		-g \
		-O0 \
		-fno-omit-frame-pointer \
		-fsanitize=address,undefined \
		-Wno-newline-eof \
		-Wno-unused-function \
		-Wno-deprecated-declarations \
		-Wno-gnu-zero-variadic-macro-arguments \
		-D_STACK_TRACE \
		-D_DEBUG
    LDFLAGS := \
		-g \
		-rdynamic \
		-O0 \
		-fno-omit-frame-pointer \
		-fsanitize=address,undefined
else
    $(error Unknown MODE=$(MODE). Use release, debug or extra-debug)
endif

# Combine with base
CFLAGS += $(BASE_CFLAGS)
LDFLAGS += $(BASE_LDFLAGS)

# Allow extras from command line
CFLAGS += $(EXTRA_CFLAGS)
LDFLAGS += $(EXTRA_LDFLAGS)

# Dirs
SRC_DIR := src
OBJ_DIR := build/obj
BIN_DIR := build
BIN := $(BIN_DIR)/main

# Sources/objects (recursive)
SOURCES := $(shell find $(SRC_DIR) -name '*.c')
OBJECTS := $(SOURCES:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)
DEPS := $(OBJECTS:.o=.d)

# Silent mode
ifeq ($(SILENT),1)
    SILENT_PREFIX := @
else
    SILENT_PREFIX :=
endif

# Include deps
-include $(DEPS)

# Rules
all: $(BIN)

$(BIN): $(OBJECTS)
	$(SILENT_PREFIX)mkdir -p $(@D)
	$(SILENT_PREFIX)$(CC) $(OBJECTS) $(LDFLAGS) -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(SILENT_PREFIX)mkdir -p $(@D)
	$(SILENT_PREFIX)$(CC) $(CFLAGS) -MMD -MP -c $< -o $@

# compile_commands.json for clangd
compile_commands:
	@echo '[' > compile_commands.json
	@for src in $(SOURCES); do \
		obj=$$(echo $$src | sed 's/$(SRC_DIR)/$(OBJ_DIR)/' | sed 's/\.c/\.o/'); \
		echo '{ "directory": "$(PWD)", "command": "$(CC) $(CFLAGS) -c '$$src' -o '$$obj'", "file": "'$$src'" },'; \
	done >> compile_commands.json
	@sed -i '$$ s/,$$/]/' compile_commands.json

clean:
	$(SILENT_PREFIX)rm -rf $(OBJ_DIR) $(BIN_DIR)/*

clean-obj:
	$(SILENT_PREFIX)rm -rf $(OBJ_DIR)/*

rebuild: clean all

run:
	@if [ -f $(BIN) ]; then $(BIN); else echo "No executable"; fi

gdb:
	@if [ -f $(BIN) ]; then gdb $(BIN); else echo "No executable"; fi

help:
	@echo "Usage: make [TARGET] [OPTIONS]"
	@echo ""
	@echo "TARGETS:"
	@echo "    help"
	@echo "        Print this help message"
	@echo "    all"
	@echo "        Build the executable (default)"
	@echo "    rebuild"
	@echo "        Force the recompilation of the entire codebase"
	@echo "    run"
	@echo "        Run the app"
	@echo "    gdb"
	@echo "        Run the app in gdb"
	@echo "    clean"
	@echo "        Remove all build artifacts"
	@echo "    clean-obj"
	@echo "        Remove object files"
	@echo "    compile_commands"
	@echo "        Generate compile_commands.json for clangd"
	@echo ""
	@echo "OPTIONS:"
	@echo "    MODE=<str>"
	@echo "        Compilation type: release, debug, extra-debug"
	@echo "        Default: release"
	@echo "    SILENT=1"
	@echo "        Silence the compilation"
	@echo "    EXTRA_CFLAGS=\"<str>\""
	@echo "        Add custom compiler flags"
	@echo "    EXTRA_LDFLAGS=\"<str>\""
	@echo "        Add custom linker flags"

.PHONY: all clean clean-obj rebuild run gdb compile_commands help