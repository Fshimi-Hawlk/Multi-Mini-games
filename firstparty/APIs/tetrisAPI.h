/**
 * @file tetrisAPI.h
 * @brief Public API for the Tetris mini-game
 * @author Multi Mini-Games Team
 * @date February 2026
 */

#ifndef TETRIS_API_H
#define TETRIS_API_H

#include "gameError.h"
#include "gameConfig.h"
#include <stdbool.h>

typedef struct TetrisGame_St TetrisGame_St;

TetrisGame_St* tetris_initGame(const GameConfig_St* config);
void tetris_gameLoop(TetrisGame_St* const game);
void tetris_freeGame(TetrisGame_St* game);
bool tetris_isRunning(const TetrisGame_St* game);

#endif // TETRIS_API_H
