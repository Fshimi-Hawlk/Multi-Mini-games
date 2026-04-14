/**
    @file solitaireAPI.h
    @author Maxime CHAUVEAU
    @date 2026-02-01
    @date 2026-04-14
    @brief Public API for the Solitaire (Klondike) mini-game
*/
#ifndef SOLITAIRE_API_H
#define SOLITAIRE_API_H

#include "APIs/generalAPI.h"
#include "systemSettings.h"
#include <stdbool.h>

/**
    @brief Definition of typedef struct SolitaireGame_St SolitaireGame_St
*/
typedef struct SolitaireGame_St SolitaireGame_St;

/**
    @brief Solitaire configuration
*/
typedef struct {
    char _;     ///< Dummy member
} SoloCardsConfig_St;

/**
    @brief Convenience macro using C99 compound literal syntax.
*/
#define solitaire_initGame(game, ...) \
    solitaire_initGame__full((game), (SoloCardsConfig_St) {._ = 0, __VA_ARGS__})

/**
    @brief Allocates and initializes a new Solitaire game instance.

    @param[out]    game_ptr  Double pointer receiving the new game handle
    @param[in]     config    Optional video/audio configuration (NULL = defaults)

    @return                  OK on success, ERROR_ALLOC on allocation failure, ERROR_ASSET_LOAD if card assets cannot be loaded
*/
Error_Et solitaire_initGame__full(SolitaireGame_St** game_ptr, const SoloCardsConfig_St config);

/**
    @brief Runs one frame: update + render.

    Sets base.running = false when the user presses ESC or closes the window.

    @param[in,out] game      Valid game instance handle
*/
void solitaire_gameLoop(SolitaireGame_St* const game);

/**
    @brief Frees all resources and sets the pointer to NULL.

    @param[in,out] game_ptr  Pointer to the game handle (set to NULL after cleanup)

    @return                  OK on success, ERROR_NULL_POINTER if game_ptr or *game_ptr is NULL
*/
Error_Et solitaire_freeGame(SolitaireGame_St** game_ptr);

/**
    @brief Checks if the game is still running.

    @param[in]     game      Game instance (may be NULL)
    @return                  True if running
*/
bool solitaire_isRunning(const SolitaireGame_St* game);

#endif // SOLITAIRE_API_H
