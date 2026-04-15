/**
    @file app.h
    @author Fshimi-Hawlk
    @date 2026-01-07
    @date 2026-04-14
    @brief Application initialization and cleanup.
*/
#ifndef SETUPS_APP_H
#define SETUPS_APP_H

#include "baseTypes.h"

/**
    @brief Initializes and loads all application fonts.

    @return true if successful, false otherwise.
*/
bool polyBlast_initFonts(void);

/**
    @brief Frees all loaded fonts.
*/
void polyBlast_freeFonts(void);

/**
    @brief Initializes the app: RNG, window, fonts.

 * Seeds PRNG, opens window, loads fonts and initialize game.
*/
bool polyBlast_initApp(void);

/**
    @brief Frees application resources and closes the window.
*/
void polyBlast_freeApp(void);

#endif // SETUPS_APP_H