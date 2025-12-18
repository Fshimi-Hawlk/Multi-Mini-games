# Dossiers
SRC_DIR := src
INCLUDE_DIR := include
BUILD_DIR := build
OBJ_DIR := $(BUILD_DIR)/obj
THIRD_PARTY := ../thirdparty/raylib/raylib-5.5_linux_amd64

# Nom de l'exécutable
TARGET := $(BUILD_DIR)/main

# Sources et objets
SRCS := $(wildcard $(SRC_DIR)/*.c)
OBJS := $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRCS))

# Flags
CFLAGS := -I$(THIRD_PARTY)/include -I$(INCLUDE_DIR)
LDFLAGS := $(THIRD_PARTY)/lib/libraylib.a -lGL -lm -lpthread -ldl -lrt -lX11

# Règles
all: $(TARGET)

# Linkage des fichiers o
$(TARGET): $(OBJS) | $(BUILD_DIR)
	gcc -o $@ $^ $(LDFLAGS) -Wall

# Compilation des .c en .o
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	gcc $(CFLAGS) -c -Wall $< -o $@

# Création des dossiers si nécessaire
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

# Nettoyage
clean:
	rm -rf $(BUILD_DIR)

.PHONY: all clean
