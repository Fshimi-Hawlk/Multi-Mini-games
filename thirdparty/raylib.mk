# ==============================================================================
# raylib.mk — centralized raylib static link configuration
#
# Included by every platform/linux.mk and platform/wsl.mk.
# Override RAYLIB_LIB_DIR before including this file if needed.
#
# Raylib 5.5 static link requires on Linux/WSL:
#   -lGL -lm -lpthread -ldl
# ==============================================================================

RAYLIB_STATIC_LDFLAGS := \
	-L$(RAYLIB_LIB_DIR) \
	-l:libraylib.a \
	-lGL \
	-lm \
	-lpthread \
	-ldl
