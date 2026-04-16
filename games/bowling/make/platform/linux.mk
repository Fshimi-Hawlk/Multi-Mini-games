RAYLIB_LIB_DIR := ../../thirdparty/libs/raylib-5.5_linux_amd64

# Base flags (always present)
BASE_CFLAGS := \
	-Wno-newline-eof \
	-Iinclude \
	-I../../thirdparty \
	-I../../firstparty

# Linker base
BASE_LDFLAGS := \
	-L$(RAYLIB_LIB_DIR) \
	-L../../firstparty/build/lib \
	-l:libraylib.a \
	-lfirstparty \
	-lGL \
	-lm \
	-lpthread \
	-ldl
