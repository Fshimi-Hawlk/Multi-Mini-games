/**
 * @file rubikAPI.h
 * @brief Public interface for the Rubik's Cube mini-game.
 */

#ifndef RUBIK_API_H
#define RUBIK_API_H

#include "APIs/generalAPI.h"

typedef struct {
    BaseGame_St base;
} RubikGame_St;

Error_Et rubik_initGame(RubikGame_St** game);
Error_Et rubik_freeGame(RubikGame_St** game);

#endif
