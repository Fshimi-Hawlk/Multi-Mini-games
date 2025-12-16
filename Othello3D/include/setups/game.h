#ifndef SETUPS_GAME_H
#define SETUPS_GAME_H

#include "utils/types.h"

/**
 * @brief Initializes the game state, camera, and 3D highlight model.
 *
 * Sets up the Othello board with starting pieces, configures the initial
 * free camera position and parameters, saves the initial camera state,
 * generates and loads the torus mesh/model used for move highlights,
 * and hides the cursor.
 *
 * @param board The game board array to initialize.
 */
void initGame(Board_t board);

#endif // SETUPS_GAME_H