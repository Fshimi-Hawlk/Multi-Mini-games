#ifndef CONFIGS_H
#define CONFIGS_H

#define WINDOW_TITLE "Block Blast"
#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 800 // 600

#define APP_BACKGROUND_COLOR (color32) {.r = 18, .g = 18, .b = 18, .a = 255}
#define BOARD_EMPTY_TILE_COLOR (color32) {.r = 22, .g = 16, .b = 79, .a = 255}
#define BLOCK_OUTLINE_COLOR (color32) {.r = 22, .g = 16, .b = 36, .a = 255}

#define MAX_FONT_COUNT     12
#define APP_TEXT_FONT_SIZE 32

#define MAX_BLOCK_PER_SHAPE 9
#define BLOCK_PX_SIZE 50
#define OUTLINE_PX_SIZE ((BLOCK_PX_SIZE) / 10.0f)

#define BOARD_WIDTH  8
#define BOARD_HEIGHT 8

#endif // CONFIGS_H