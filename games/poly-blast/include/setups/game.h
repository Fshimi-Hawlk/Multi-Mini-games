/**
    @file game.h
    @author Kimi BERGE
    @date 2026-01-07
    @date 2026-04-14
    @brief Full game state initialization.
*/
#ifndef SETUPS_GAME_H
#define SETUPS_GAME_H

#include "utils/userTypes.h"

/**
    @brief Initialises a fresh board (position, clearing arrays, etc.).
*/
bool polyBlast_initBoard(Board_St* const board);

/**
    @brief Description for polyBlast_initPrefabManager
    @param[in,out] manager The manager parameter
    @return Success/failure or the result of the function
*/
bool polyBlast_initPrefabManager(PrefabManager_St* const manager);

/**
    @brief Full game initialisation (prefabs, board, slots, scoring).
*/
bool polyBlast_initGame(PolyBlastGame_St* const game);

/**
    @brief Resets the game to a completely fresh state (new game / restart).

    - Board is emptied and re-positioned
    - Score/streak/grace are zeroed and texts rebuilt
    - Prefab manager is fully re-initialised (bags + weighted slots)
    - placementSimulation is called for a new set of 3 shapes
    - gameOver flag is cleared

    @param game  Game state to reset (modified in-place)
    @pre  game != NULL
    @post  game->gameOver == false, board empty, fresh slots ready
*/
void polyBlast_resetGame(PolyBlastGame_St* const game);

#endif // SETUPS_GAME_H