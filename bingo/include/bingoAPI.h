/**
    @file bingoAPI.h
    @author Fshimi-Hawlk
    @date 2026-01-25
    @date 2026-03-19
    @brief Public API for Bingo mini-game.
*/

#ifndef BINGO_API_H
#define BINGO_API_H

#include "baseTypes.h"
#include "APIs/generalAPI.h"

// Forward declaration of the internal state structure
struct BingoGame_St;

/**
    @brief Configuration parameters for Bingo initialization.
*/
typedef struct {
    char _;             ///< Unused field
} BingoConfigs_St;

/**
    @brief Allocates and initializes a new Bingo game instance.
*/
Error_Et bingo_initGame__full(struct BingoGame_St** game, BingoConfigs_St configs);

/**
    @brief Runs one complete frame: input processing → logic update → rendering.
*/
Error_Et bingo_gameLoop(struct BingoGame_St* const game);

/**
    @brief Frees all resources allocated for a Bingo game.
*/
Error_Et bingo_freeGame(struct BingoGame_St* game);

#endif // BINGO_API_H
