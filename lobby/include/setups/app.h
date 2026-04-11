/**
    @file app.h (setups)
    @author Fshimi Hawlk
    @date 2026-01-07
    @date 2026-04-10
    @brief Application initialization and cleanup.
*/

#include "APIs/generalAPI.h"

#ifndef SETUPS_APP_H
#define SETUPS_APP_H

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