# ───────────────────────────────────────────────────────────────
# Module Makefile ─ firstparty
# ───────────────────────────────────────────────────────────────

CC ?= gcc
MODE ?= release

# Flags based on mode (simplified)
ifeq ($(MODE),release)
    CFLAGS := -O2
else
    CFLAGS := -g -O0 -D_DEBUG
endif

CFLAGS += -I. -I../thirdparty -IAPIs

# Output directories
BUILD_DIR := build
OBJ_DIR := $(BUILD_DIR)/obj
LIB_DIR := $(BUILD_DIR)/lib

LIB_NAME := firstparty
SOURCES := logger.c systemSettings.c progress.c rand.c APIs/gameRegistry.c
OBJECTS := $(SOURCES:%.c=$(OBJ_DIR)/%.o)
STATIC_LIB := $(LIB_DIR)/lib$(LIB_NAME).a

all: static-lib

static-lib: $(STATIC_LIB)

$(STATIC_LIB): $(OBJECTS)
	@mkdir -p $(@D)
	ar rcs $@ $^

$(OBJ_DIR)/%.o: %.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(BUILD_DIR)

.PHONY: all static-lib clean
