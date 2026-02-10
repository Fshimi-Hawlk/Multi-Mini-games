# Dirs
SRC_DIR := src
TEST_DIR := tests

# Library/shared sources/objects (recursive, excluding main.c)
LIB_SOURCES := $(shell find $(SRC_DIR) -name '*.c' ! -name '$(MAIN_NAME).c')
LIB_OBJECTS := $(LIB_SOURCES:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

# Main source/object
MAIN_SOURCE := $(SRC_DIR)/$(MAIN_NAME).c
MAIN_OBJECT := $(OBJ_DIR)/$(MAIN_NAME).o

# Test sources/objects/bins (recursive)
TEST_SOURCES := $(shell find $(TEST_DIR) -name '*.c')
TEST_OBJECTS := $(TEST_SOURCES:$(TEST_DIR)/%.c=$(OBJ_DIR)/tests/%.o)
TEST_BINS := $(TEST_SOURCES:$(TEST_DIR)/%.c=$(TEST_BIN_DIR)/%)

# All deps
DEPS := $(LIB_OBJECTS:.o=.d) $(MAIN_OBJECT:.o=.d) $(TEST_OBJECTS:.o=.d)
