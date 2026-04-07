RAYLIB_LIB_DIR := ../../thirdparty/libs/raylib-5.5_win64_mingw

BASE_CFLAGS := \
    -Iinclude \
    -Isrc \
    -I../../thirdparty \
    -I../../firstparty \
    -DPLATFORM_DESKTOP

BASE_LDFLAGS := \
    -L$(RAYLIB_LIB_DIR) \
    -lraylib \
    -lopengl32 \
    -lgdi32 \
    -lwinmm \
    -static
