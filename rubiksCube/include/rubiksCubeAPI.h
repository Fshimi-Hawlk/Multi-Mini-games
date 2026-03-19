/**
 * @file rubiksCubeAPI.h
 * @brief Public API for the Rubik's Cube mini-game
 * @author Maxime CHAUVEAU
 * @date March 2026
 * 
 * @section overview Overview
 * This module provides a complete Rubik's Cube 3D puzzle game implementation
 * using the raylib graphics library. The game features a 3x3 Rubik's Cube that
 * players can rotate and manipulate to solve the puzzle.
 * 
 * @section usage Usage
 * @code
 * RubiksCubeGame_St* game = rubiksCube_initGame(NULL);  // NULL = default config
 * if (!game) { handle error }
 * while (rubiksCube_isRunning(game)) rubiksCube_gameLoop(game);
 * rubiksCube_freeGame(game);
 * @endcode
 * 
 * @section controls Controls
 * - Arrow Keys: Rotate the view around the cube
 * - F, B, R, L, U, D: Rotate the corresponding face clockwise
 * - Shift + key: Rotate the face counter-clockwise
 * - SPACE: Scramble the cube
 * - ESC: Quit the game
 * 
 * @section cube_notation Cube Notation
 * The standard Rubik's Cube notation is used:
 * - F (Front): Front face
 * - B (Back): Back face
 * - R (Right): Right face
 * - L (Left): Left face
 * - U (Up): Up face
 * - D (Down): Down face
 */

#ifndef RUBIKS_CUBE_API_H
#define RUBIKS_CUBE_API_H

#include "APIs/gameError.h"
#include "APIs/gameConfig.h"
#include <stdbool.h>

/**
 * @brief Rubik's Cube game instance structure
 * 
 * This opaque structure contains all the internal state of the game,
 * including the cube configuration, game state, and rendering data.
 * Users should only interact with this structure through the API functions.
 */
typedef struct RubiksCubeGame_St RubiksCubeGame_St;

/**
 * @brief Initialize a new Rubik's Cube game
 * 
 * Creates and initializes a new Rubik's Cube game instance with the specified
 * configuration. If config is NULL, default settings will be used.
 * 
 * @param config Game configuration (can be NULL for default settings)
 * @return Pointer to the game instance, or NULL on failure
 * 
 * @note The game window must be initialized before calling this function
 * @note The returned instance must be freed using rubiksCube_freeGame()
 * 
 * @see rubiksCube_freeGame
 * @see rubiksCube_gameLoop
 */
RubiksCubeGame_St* rubiksCube_initGame(const GameConfig_St* config);

/**
 * @brief Run one frame of the game loop
 * 
 * Executes a single frame of the game, handling input, updating game state,
 * and rendering the graphics. This function should be called in a loop
 * while the game is running.
 * 
 * @param game Game instance
 * 
 * @note This function handles all input processing and rendering
 * @warning Passing NULL for game will result in undefined behavior
 * 
 * @see rubiksCube_isRunning
 * @see rubiksCube_initGame
 */
void rubiksCube_gameLoop(RubiksCubeGame_St* const game);

/**
 * @brief Free the game resources
 * 
 * Releases all memory and resources associated with the game instance.
 * After calling this function, the game pointer is invalid and should
 * not be used.
 * 
 * @param game Game instance to free
 * 
 * @note This function is safe to call with NULL
 * @note Always call this before exiting to prevent memory leaks
 * 
 * @see rubiksCube_initGame
 */
void rubiksCube_freeGame(RubiksCubeGame_St* game);

/**
 * @brief Check if the game is still running
 * 
 * Queries the current running state of the game. The game is considered
 * running until the player quits or solves the cube.
 * 
 * @param game Game instance
 * @return true if game is running, false otherwise
 * 
 * @note Returns false if game is NULL
 * 
 * @see rubiksCube_gameLoop
 * @see rubiksCube_initGame
 */
bool rubiksCube_isRunning(const RubiksCubeGame_St* game);

#endif // RUBIKS_CUBE_API_H
