/**
    @file app.h (setups)
    @author Fshimi Hawlk
    @date 2026-01-07
    @date 2026-03-03
    @brief Application initialization and cleanup.
*/

#ifndef SETUPS_APP_H
#define SETUPS_APP_H

#include "stdbool.h"

/**
    @brief Initializes and loads all application fonts.

    @return true if successful, false otherwise.
*/
bool bingo_initFonts(void);

/**
    @brief Frees all loaded fonts.
*/
void bingo_freeFonts(void);

/**
    @brief Initializes the app: RNG, window, fonts.

 * Seeds PRNG, opens window, loads fonts and initialize game.
*/
bool bingo_initApp(void);

/**
    @brief Frees application resources and closes the window.
*/
void bingo_freeApp(void);

#endif // SETUPS_APP_H