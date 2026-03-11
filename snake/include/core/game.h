#include "utils/common.h"

bool isOOB(iVector2 coord);
bool selfCollision(int bodyLength, iVector2 headCoord);
void updateBody(int board[SIZE_BOARD][SIZE_BOARD], int bodyLength);

int initBoard(int board[SIZE_BOARD][SIZE_BOARD], int bodyLength);

void spawnApple(int board[SIZE_BOARD][SIZE_BOARD], iVector2* appleCoord);

void writeRecord(int highScore);

int readRecord(void);
bool mouvement(iVector2* direction);