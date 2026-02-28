#ifndef UTILS_CONFIGS_H
#define UTILS_CONFIGS_H

#ifndef ASSET_PATH
#define ASSET_PATH "assets/"
#endif

#define WINDOW_WIDTH    400
#define WINDOW_HEIGHT   800
#define WINDOW_NAME     "Tetris"

#define BOARD_WIDTH 10
#define BOARD_HEIGHT 20

#define CELL_SIZE 25
#define RGB(r, g, b) (Color) {r, g, b, 255}
#define BACKGROUND_COLOR RGB(0, 0, 30)
#define BOARD_BACKGROUND_COLOR RGB(50, 50, 50)
#define BOARD_GRID_COLOR RGB(100, 100, 100)

#define CYAN RGB(0, 255, 255)

#define INITIAL_DROP_DELAY 1.0f
#define MIN_DROP_DELAY 0.1f
#define SOFT_DROP_MULTIPLIER 20.0f

#define INPUT_INITIAL_DELAY 0.2f
#define INPUT_REPEAT_DELAY 0.05f

#endif