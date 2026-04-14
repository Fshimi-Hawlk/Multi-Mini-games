/**
    @file app.h
    @author Kimi BERGE
    @date 2026-01-07
    @date 2026-04-14
    @brief Application-level initialization and cleanup for the Bingo game.
*/
#ifndef SETUPS_APP_H
#define SETUPS_APP_H

#include "stdbool.h"

/**
    @brief Initializes and loads all application fonts.

    @return                     True if successful, false otherwise.
*/
bool bingo_initFonts(void);

/**
    @brief Frees all loaded fonts.
*/
void bingo_freeFonts(void);

/**
    @brief Initializes the application: RNG, window, and fonts.

    Seeds the PRNG, opens the window, loads fonts and initializes the game.

    @return                     True if initialization succeeded, false otherwise.
*/
bool bingo_initApp(void);

/**
    @brief Frees application-wide resources and closes the window.
*/
void bingo_freeApp(void);

#endif // SETUPS_APP_H