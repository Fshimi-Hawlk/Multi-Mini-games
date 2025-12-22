#ifndef CONFIG_H
#define CONFIG_H

#define WINDOW_TITLE "Game Selector"
#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

#define BOARD_SIZE 8
#define CELL_PX_SIZE ((float)WINDOW_HEIGHT / (BOARD_SIZE + 2))
#define BOARD_PX_SIZE ((BOARD_SIZE) * (CELL_PX_SIZE))

#define MAX_FONT_COUNT     12
#define APP_TEXT_FONT_SIZE 32

#define CAMERA_SENSITIVITY 0.15f

#define MAX_LIGHTS 4
#define LIGHT_DIRECTIONAL 0

#endif // CONFIG_H