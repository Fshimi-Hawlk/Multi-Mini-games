/**
    @file game.h
    @brief Core Suika game logic header
    @author Multi Mini-Games Team
    @date February 2026

    Declares all core game functions for the Suika (Watermelon Game) mini-game.
    These functions handle initialization, update, rendering, and cleanup.
*/

#ifndef SUIKA_CORE_GAME_H
#define SUIKA_CORE_GAME_H

#include "utils/types.h"

/**
    @brief Initialize the Suika game state.

    Sets up initial game state including score, fruit pool, and first fruit.
    Must be called after suika_loadAssets().

    @param game Pointer to the game state to initialize
*/
void suika_init(SuikaGame_St* game);

/**
    @brief Update game state for one frame.

    Handles input, physics simulation, collision detection, and game over checks.

    @param game Pointer to the game state
    @param deltaTime Time elapsed since last frame in seconds
*/
void suika_update(SuikaGame_St* game, float deltaTime);

/**
    @brief Render the current game state.

    Draws background, container, all active fruits, and HUD.

    @param game Pointer to the game state (const - does not modify)
*/
void suika_draw(const SuikaGame_St* game);

/**
    @brief Clean up game resources.

    Releases any resources held by the game state.
    Called before freeing the game structure.

    @param game Pointer to the game state
*/
void suika_cleanup(SuikaGame_St* game);

/**
    @brief Get properties for a fruit type.

    Returns radius, color, points, and sprite rectangle for the given fruit type.

    @param type The fruit type to query
    @return Pointer to the properties structure (valid for program lifetime)
*/
const FruitProperties_St* suika_getFruitProperties(FruitType_Et type);

/**
    @brief Spawn the next fruit to be dropped.

    Randomly selects a small fruit type (0-4) and prepares it for dropping.

    @param game Pointer to the game state
*/
void suika_spawnNextFruit(SuikaGame_St* game);

/**
    @brief Drop the next fruit into play.

    Activates the preview fruit and places it at the current drop position.

    @param game Pointer to the game state
*/
void suika_dropFruit(SuikaGame_St* game);

/**
    @brief Run physics simulation for one frame.

    Updates velocities, positions, and handles collisions between fruits
    and with container walls.

    @param game Pointer to the game state
    @param deltaTime Time elapsed since last frame in seconds
*/
void suika_updatePhysics(SuikaGame_St* game, float deltaTime);

/**
    @brief Check for and process fruit merges.

    Detects collisions between same-type fruits and merges them
    into the next larger fruit type.

    @param game Pointer to the game state
*/
void suika_checkMerging(SuikaGame_St* game);

/**
    @brief Check for game over condition.

    Detects if fruits have been above the drop line for too long.

    @param game Pointer to the game state
*/
void suika_checkGameOver(SuikaGame_St* game);

/**
    @brief Reset the game to initial state.

    Clears all fruits, resets score, and prepares for a new game.
    Keeps high score intact.

    @param game Pointer to the game state
*/
void suika_reset(SuikaGame_St* game);

/**
    @brief Draw the heads-up display.

    Renders score, high score, game over message, and instructions.

    @param game Pointer to the game state (const - does not modify)
*/
void suika_drawHUD(const SuikaGame_St* game);

/**
    @brief Load game assets.

    Loads the fruit sprite atlas texture from disk.

    @param game Pointer to the game state
*/
void suika_loadAssets(SuikaGame_St* game);

/**
    @brief Unload game assets.

    Releases the fruit sprite atlas texture.

    @param game Pointer to the game state
*/
void suika_unloadAssets(SuikaGame_St* game);

#endif
