/**
    @file game.h (setups)
    @author Fshimi Hawlk
    @date 2026-01-07
    @brief Full game state initialization.
*/

#ifndef SETUPS_GAME_H
#define SETUPS_GAME_H

#include "utils/userTypes.h"

bool initBoard(Board_St* const board);

/**
    @brief Initializes the game: prefabs, board, and state.

    Sets up board, fills prefab bags, shuffles initial slots.
*/
bool initGame(GameState_St* const game, bool fromLoad);

#endif // SETUPS_GAME_H