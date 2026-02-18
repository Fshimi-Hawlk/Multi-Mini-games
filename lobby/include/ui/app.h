/**
 * @file app.h
 * @author i-Charlys (CAILLON Charles)
 * @date 2026-03-18
 * @brief UI functions for general application interface elements.
 */

#ifndef UI_APP_H
#define UI_APP_H

#include "utils/userTypes.h"

/**
 * @brief Renders the texture selection menu.
 */
void drawMenuTextures(const LobbyGame_St* const game);

/**
 * @brief Renders the skin selection button.
 */
void drawSkinButton(void);

#endif // UI_APP_H