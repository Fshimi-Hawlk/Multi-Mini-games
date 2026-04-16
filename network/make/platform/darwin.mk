RAYLIB_LIB_DIR := ../thirdparty/libs/raylib-5.5_macos

BASE_CFLAGS := \
    -Iinclude \
    -I../thirdparty \
    -I../firstparty

BASE_LDFLAGS := \
    -L$(RAYLIB_LIB_DIR) \
    -lraylib \
    -framework OpenGL \
    -framework Cocoa \
    -framework IOKit \
    -framework CoreVideo \
    -framework CoreAudio