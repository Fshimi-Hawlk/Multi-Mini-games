RAYLIB_LIB_DIR := ../../thirdparty/libs/raylib-5.5_linux_amd64

BASE_CFLAGS := \
	-Iinclude \
	-Isrc \
	-I../../thirdparty \
	-I../../firstparty \
	-DPLATFORM_WSL \
	-D_BSD_SOURCE

BASE_LDFLAGS := \
	-L$(RAYLIB_LIB_DIR) \
	-l:libraylib.a \
	-lGL \
	-lm \
	-lpthread \
	-ldl
