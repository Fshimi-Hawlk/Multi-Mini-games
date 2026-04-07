#ifndef PUISSANCE4_CORE_GAME_H
#define PUISSANCE4_CORE_GAME_H

#include <stdbool.h>
#include <stddef.h>
#include "../../firstparty/APIs/gameConfig.h"

#define PUISSANCE4_BOARD_ROWS 6
#define PUISSANCE4_BOARD_COLS 7

typedef enum {
    PUISSANCE4_PLAYER_NONE = 0,
    PUISSANCE4_PLAYER_1 = 1,
    PUISSANCE4_PLAYER_2 = 2
} Puissance4Player_Et;

typedef enum {
    PUISSANCE4_STATE_MENU,
    PUISSANCE4_STATE_PLAYING,
    PUISSANCE4_STATE_GAME_OVER
} Puissance4State_Et;

typedef struct Puissance4Game_St Puissance4Game_St;

Puissance4Game_St* puissance4Core_initGame(const GameConfig_St* config);
void puissance4Core_gameLoop(Puissance4Game_St* const game);
void puissance4Core_freeGame(Puissance4Game_St* game);
bool puissance4Core_isRunning(const Puissance4Game_St* game);

int puissance4Core_getBoard(const Puissance4Game_St* game, int row, int col);
Puissance4State_Et puissance4Core_getState(const Puissance4Game_St* game);
Puissance4Player_Et puissance4Core_getCurrentPlayer(const Puissance4Game_St* game);
Puissance4Player_Et puissance4Core_getWinner(const Puissance4Game_St* game);
int puissance4Core_getSelectedColumn(const Puissance4Game_St* game);

#endif