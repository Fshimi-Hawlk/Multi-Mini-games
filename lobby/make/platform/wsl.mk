RAYLIB_LIB_DIR := ../thirdparty/libs/raylib-5.5_linux_amd64

# Base flags (always present)
BASE_CFLAGS := \
	-Iinclude \
	-I../thirdparty \
	-I../firstparty \
	-I../games/snake \
	-I../games/suika \
	-I../games/solo-cards \
	-I../games/mini-golf \
	-I../games/bowling \
	-DPLATFORM_WSL

# Linker base (WSL = Linux + explicit pthread + libdl)
BASE_LDFLAGS := \
	-L$(RAYLIB_LIB_DIR) \
	-l:libraylib.a \
	-lGL \
	-lm \
	-lpthread \
	-ldl
