/**
    @file rubikAPI.h
    @author Léandre BAUDET
    @date 2026-04-14
    @date 2026-04-14
    @brief Public interface for the Rubik's Cube mini-game.
*/
#ifndef RUBIK_API_H
#define RUBIK_API_H

#include "APIs/generalAPI.h"
#include "networkInterface.h"

/**
    @brief Definition of enum RubikActionCodes_e
*/
enum RubikActionCodes_e {
    ACTION_CODE_RUBIK_SCRAMBLE = firstAvailableActionCode + 0x20,
    ACTION_CODE_RUBIK_PROGRESS,
    ACTION_CODE_RUBIK_ELIMINATE
};

/**
    @brief Definition of typedef struct
*/
typedef struct {
    BaseGame_St base;
} RubikGame_St;

/**
    @brief Description for rubik_initGame
    @param[in,out] game The game parameter
    @return Success/failure or the result of the function
*/
Error_Et rubik_initGame(RubikGame_St** game);
/**
    @brief Description for rubik_freeGame
    @param[in,out] game The game parameter
    @return Success/failure or the result of the function
*/
Error_Et rubik_freeGame(RubikGame_St** game);

#endif
