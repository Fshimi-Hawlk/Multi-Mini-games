#include "utils/common.h"
#include "snakeAPI.h"

int main(void) {
    srand(time(NULL));
    SetTraceLogLevel(LOG_WARNING);
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Snake");
    SetTargetFPS(60);

    SnakeGame_St* game = NULL;
    snake_initGame(game);
    snake_gameLoop(game);
    snake_freeGame(&game);

    CloseWindow();
    return 0;
}