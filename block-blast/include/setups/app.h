/**
 * @file app.h (setups)
 * @author Fshimi Hawlk
 * @date 2026-01-07
 * @brief Application initialization and cleanup.
 */

#ifndef SETUPS_APP_H
#define SETUPS_APP_H

#include "utils/baseTypes.h"

/**
 * @brief Initializes and loads all application fonts.
 *
 * @return true if successful, false otherwise.
 */
bool8 initFonts(void);

/**
 * @brief Frees all loaded fonts.
 */
void freeFonts(void);

/**
 * @brief Initializes the application, including window, RNG, and game setup.
 *
 * @return true if successful, false otherwise.
 */
bool8 initApp(void);

/**
 * @brief Frees application resources and closes the window.
 */
void freeApp(void);

#endif // SETUPS_APP_H