/**
    @file gameAPI.h
    @author Maxime CHAUVEAU
    @date 2026-02-01
    @date 2026-04-14
    @brief Public API for the Suika mini-game.
*/
#ifndef SUIKA_GAME_API_H
#define SUIKA_GAME_API_H

#include "APIs/generalAPI.h"

/**
    @brief Definition of typedef struct SuikaGame_St SuikaGame_St
*/
typedef struct SuikaGame_St SuikaGame_St;

/**
    @brief Configuration parameters for the Suika game.
*/
typedef struct
{
    unsigned int fps; ///< Target frames per second
} SuikaConfigs_St;

/**
    @brief Initialize the Suika game with default or custom configurations.

    Helper macro that wraps suika_initGame__full with default values.

    @param[out]    game  Pointer to the game handle to initialize
    @param[in]     ...   Optional configuration fields (e.g., .fps = 120)
    @return              Error_Et code
*/
#define suika_initGame(game, ...) \
    suika_initGame__full((game), (SuikaConfigs_St){ .fps = 60, __VA_ARGS__ })

/**
    @brief Core initialization function for the Suika game.

    Allocates memory for the game state and sets up initial values.

    @param[out]    game_ptr Double pointer to the game handle to be allocated
    @param[in]     configs  Initial game configurations
    @return                 SUCCESS if initialized correctly, error code otherwise
*/
Error_Et suika_initGame__full(SuikaGame_St** game_ptr, SuikaConfigs_St configs);

/**
    @brief Main loop for the Suika game.

    Handles one frame of game logic and rendering.

    @param[in,out] game Pointer to the game handle
    @return             SUCCESS unless the game should terminate or an error occurs
*/
Error_Et suika_gameLoop(SuikaGame_St* const game);

/**
    @brief Free all resources associated with the Suika game.

    Cleans up game state and sets the handle to NULL.

    @param[in,out] game Double pointer to the game handle to be freed
    @return             SUCCESS if freed correctly, error code otherwise
*/
Error_Et suika_freeGame(SuikaGame_St** game);

#endif
