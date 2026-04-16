RAYLIB_LIB_DIR := ../thirdparty/libs/raylib-5.5_linux_amd64

# Base flags (always present)
BASE_CFLAGS := \
	-Iinclude \
	-I../thirdparty \
	-I../firstparty \

# Linker base
BASE_LDFLAGS := \
	-L$(RAYLIB_LIB_DIR) \
	-l:libraylib.a \
	-lm