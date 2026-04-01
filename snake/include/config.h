#ifndef SNAKE_CONFIG_H
#define SNAKE_CONFIG_H

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

#define SIZE_BOARD 20
#define MAX_LENGTH 400

#define CELL_SIZE (WINDOW_HEIGHT / SIZE_BOARD)

#define BACKGROUND_COLOR WHITE

enum {
    GRASS, HEAD, BODY, APPLE
};

typedef struct {
    int x, y;
} iVector2_st;

typedef struct {
    float timer;
    float delay;
} speed_st;

#define SOUNDS_PATH "assets/sounds/"

#endif