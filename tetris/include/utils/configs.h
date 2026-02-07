#ifndef UTILS_CONFIGS_H
#define UTILS_CONFIGS_H

#ifndef ASSET_PATH
#define ASSET_PATH "assets/"
#endif

#define WINDOW_WIDTH    800
#define WINDOW_HEIGHT   600
#define WINDOW_NAME     "Tetris"

#define BOARD_WIDTH 10
#define BOARD_HEIGHT 20

#define CELL_SIZE 25
#define RGB(r, g, b) (Color) {r, g, b, 255}
#define BACKGROUND_COLOR RGB(0, 0, 30)
#define BOARD_BACKGROUND_COLOR RGB(50, 50, 50)
#define BOARD_GRID_COLOR RGB(100, 100, 100)

#define CYAN RGB(0, 255, 255)

#endif