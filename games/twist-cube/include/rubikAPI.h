/**
 * @file rubikAPI.h
 * @brief Public interface for the Rubik's Cube mini-game.
 */

#ifndef RUBIK_API_H
#define RUBIK_API_H

#include "APIs/generalAPI.h"
#include "networkInterface.h"

enum RubikActionCodes_e {
    ACTION_CODE_RUBIK_SCRAMBLE = firstAvailableActionCode + 0x20,
    ACTION_CODE_RUBIK_PROGRESS,
    ACTION_CODE_RUBIK_ELIMINATE
};

typedef struct {
    BaseGame_St base;
} RubikGame_St;

Error_Et rubik_initGame(RubikGame_St** game);
Error_Et rubik_freeGame(RubikGame_St** game);

#endif
