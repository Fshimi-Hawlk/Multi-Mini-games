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
	-I../games/bowling

# Linker base
# -l:libraylib.a  : bundled raylib 5.5 static (avoids system version conflicts)
# -lGL            : OpenGL (required by raylib on Linux)
# -lm -lpthread -ldl : standard C + threading + dynamic linking
BASE_LDFLAGS := \
	-L$(RAYLIB_LIB_DIR) \
	-l:libraylib.a \
	-lGL \
	-lm \
	-lpthread \
	-ldl
