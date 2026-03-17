#ifndef CORE_GAME_H
#define CORE_GAME_H

#include "utils/userTypes.h"

bool isOOB(iVector2 coord);
bool isSnakeEmpty(Snake_St* snake);

void initSnake(Snake_St* snake);

bool selfCollision(const Snake_St* const snake, iVector2 nextHeadPos);
void updateBoard(Board_t board, const Snake_St* const snake);

int initBoard(Board_t board, const Snake_St* const snake);

void spawnApple(Board_t board);

void writeRecord(int highScore);

int readRecord(void);
bool mouvement(iVector2* direction);

void snakeAppend(Snake_St* snake, iVector2 vector);
void snakeRemove(Snake_St* snake, iVector2* vector);
void freeSnake(Snake_St* snake);

#endif // CORE_GAME_H