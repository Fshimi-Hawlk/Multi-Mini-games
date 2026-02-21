RAYLIB_LIB_DIR := ../thirdparty/libs/raylib-5.5_win64_mingw

BASE_CFLAGS := \
    -Iinclude \
    -I../thirdparty \
    -I../firstparty \
    -DPLATFORM_DESKTOP   # raylib sometimes needs this on Windows

BASE_LDFLAGS := \
    -L$(RAYLIB_LIB_DIR) \
    -lraylib \
    -lopengl32 \
    -lgdi32 \
    -lwinmm \
    -static