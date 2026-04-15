/**
    @file game.h
    @author Maxime CHAUVEAU
    @date 2026-02-01
    @date 2026-04-14
    @brief Core Suika game logic header
*/
#ifndef SUIKA_CORE_GAME_H
#define SUIKA_CORE_GAME_H

#include "utils/types.h"

/**
    @brief Initialize the Suika game state.

    Sets up initial game state including score, fruit pool, and first fruit.
    Must be called after suika_loadAssets().

    @param[in,out] game Pointer to the game state to initialize
    @return            void
*/
void suika_initGame(SuikaGame_St* game);

/**
    @brief Update game state for one frame.

    Handles input, physics simulation, collision detection, and game over checks.

    @param[in,out] game      Pointer to the game state
    @param[in]     deltaTime Time elapsed since last frame in seconds
    @return                  void
*/
void suika_updateGame(SuikaGame_St* game, float deltaTime);

/**
    @brief Render the current game state.

    Draws background, container, all active fruits, and HUD.

    @param[in]     game Pointer to the game state (const - does not modify)
    @return             void
*/
void suika_drawGame(const SuikaGame_St* game);

/**
    @brief Get properties for a fruit type.

    Returns radius, color, points, and sprite rectangle for the given fruit type.

    @param[in]     type The fruit type to query
    @return             Pointer to the properties structure (valid for program lifetime)
*/
const FruitProperties_St* suika_getFruitProperties(FruitType_Et type);

/**
    @brief Spawn the next fruit to be dropped.

    Randomly selects a small fruit type (0-4) and prepares it for dropping.

    @param[in,out] game Pointer to the game state
    @return            void
*/
void suika_spawnNextFruit(SuikaGame_St* game);

/**
    @brief Drop the next fruit into play.

    Activates the preview fruit and places it at the current drop position.

    @param[in,out] game Pointer to the game state
    @return            void
*/
void suika_dropFruit(SuikaGame_St* game);

/**
    @brief Check for game over condition.

    Detects if fruits have been above the drop line for too long.

    @param[in,out] game Pointer to the game state
    @return            void
*/
void suika_checkGameOver(SuikaGame_St* game);

/**
    @brief Reset the game to initial state.

    Clears all fruits, resets score, and prepares for a new game.
    Keeps high score intact.

    @param[in,out] game Pointer to the game state
    @return            void
*/
void suika_reset(SuikaGame_St* game);

/**
    @brief Draw the heads-up display.

    Renders score, high score, game over message, and instructions.

    @param[in]     game Pointer to the game state (const - does not modify)
    @return             void
*/
void suika_drawHUD(const SuikaGame_St* game);

/**
    @brief Load game assets.

    Loads the fruit sprite atlas texture from disk.

    @param[in,out] game Pointer to the game state
    @return            void
*/
void suika_loadAssets(SuikaGame_St* game);

/**
    @brief Unload game assets.

    Releases the fruit sprite atlas texture.

    @param[in,out] game Pointer to the game state
    @return            void
*/
void suika_unloadAssets(SuikaGame_St* game);

#endif
