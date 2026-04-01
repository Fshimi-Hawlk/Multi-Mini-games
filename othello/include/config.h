#ifndef OTHELLO_CONFIG_H
#define OTHELLO_CONFIG_H

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

#define BOARD_SIZE 8
#define CELL_SIZE (WINDOW_HEIGHT / (BOARD_SIZE + 2))

typedef int board_t[BOARD_SIZE][BOARD_SIZE];

enum {
    EMPTY, PLAYER, COMPUTER
};

typedef struct {
    int x, y;
} pos_st;

typedef struct {
    pos_st pos;
    int score;
} validCells_st;

#define SOUNDS_PATH "assets/sounds/"

#endif