/**
    @file app.h
    @author Léandre BAUDET
    @date 2026-01-07
    @date 2026-04-14
    @brief Application initialization and cleanup.
*/
#include "APIs/generalAPI.h"

#ifndef SETUPS_APP_H
#define SETUPS_APP_H

void lobby_initFonts(void);
void lobby_freeFonts(void);

/**
    @brief Initializes the app: RNG, window, fonts.

    opens window, loads fonts and initialize game.
*/
Error_Et lobby_initApp(void);

/**
    @brief Frees application resources and closes the window.
*/
void lobby_freeApp(void);

#endif // SETUPS_APP_H