# @file wsl.mk
# @brief Platform-specific configuration for WSL (Windows Subsystem for Linux)

RAYLIB_LIB_DIR := ../thirdparty/libs/raylib-5.5_linux_amd64

# Base flags (always present)
BASE_CFLAGS := \
	-Wno-newline-eof \
	-Iinclude \
	-I../thirdparty \
	-I../firstparty \
	-DPLATFORM_WSL \
	-D_BSD_SOURCE

# Linker base - WSL uses Linux raylib but needs pthread
BASE_LDFLAGS := \
	-L$(RAYLIB_LIB_DIR) \
	-l:libraylib.a \
	-lm \
	-pthread

# WSL-specific: OpenMP for parallel processing
ifeq ($(findstring wsl-release,$(MODE)),wsl-release)
    BASE_CFLAGS += -fopenmp
    BASE_LDFLAGS += -fopenmp
endif