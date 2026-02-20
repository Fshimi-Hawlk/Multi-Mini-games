/**
 * @file app.h
 * @author Fshimi-Hawlk
 * @date 2026-01-07
 * @date 2026-02-20
 * @brief Application initialization and cleanup.
 *
 * Longer description if needed (2–5 lines max):
 *   - What this file/module does
 *   - Key types/functions it exposes
 *   - Important constraints or usage rules
 *   - Cross-references to related files (@see ...)
 *
 * @note Any noteworthy limitation, future plan or non-obvious decision
 * @see `moduleName` for [[`stuff that comes from the module`], ...]
 */

#ifndef SETUPS_APP_H
#define SETUPS_APP_H

#include "utils/common.h"

/**
 * @brief Initializes and loads all application fonts.
 *
 * @return true if successful, false otherwise.
 */
bool initFonts(void);

/**
 * @brief Frees all loaded fonts.
 */
void freeFonts(void);

/**
 * @brief Initializes the app: RNG, window, fonts.
 *
 * Seeds PRNG, opens window, loads fonts and initialize game.
 */
bool initApp(void);

/**
 * @brief Frees application resources and closes the window.
 */
void freeApp(void);

#endif // SETUPS_APP_H