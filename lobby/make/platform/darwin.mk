RAYLIB_LIB_DIR := ../thirdparty/libs/raylib-5.5_macos

BASE_CFLAGS := \
    -Iinclude \
    -I../thirdparty \
    -I../firstparty \
    -I../games/snake \
    -I../games/suika \
    -I../games/solo-cards \
    -I../games/mini-golf \
    -I../games/bowling

BASE_LDFLAGS := \
    -L$(RAYLIB_LIB_DIR) \
    -lraylib \
    -framework OpenGL \
    -framework Cocoa \
    -framework IOKit \
    -framework CoreVideo \
    -framework CoreAudio