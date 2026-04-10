#ifndef CORE_GAME_H
#define CORE_GAME_H

#include "utils/userTypes.h"

bool snake_isOOB(iVector2 coord);
bool snake_isSnakeEmpty(Snake_St* snake);

void snake_initSnake(Snake_St* snake);

bool snake_selfCollision(const Snake_St* const snake, iVector2 nextHeadPos);
void snake_updateBoard(Board_t board, const Snake_St* const snake);

int snake_initBoard(Board_t board, const Snake_St* const snake);

void snake_spawnApple(Board_t board);

void snake_writeRecord(int highScore);

int snake_readRecord(void);
bool snake_mouvement(iVector2* direction);

void snake_snakeAppend(Snake_St* snake, iVector2 vector);
void snake_snakeRemove(Snake_St* snake, iVector2* vector);
void snake_freeSnake(Snake_St* snake);

#endif // CORE_GAME_H