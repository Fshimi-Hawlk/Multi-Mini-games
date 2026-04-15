/**
    @file app.h
    @author Fshimi-Hawlk
    @date 2026-03-03
    @date 2026-04-14
    @brief Drawing functions for the game's UI and information panels.
*/
#ifndef UI_APP_H
#define UI_APP_H

#include "utils/userTypes.h"

/**
    @brief Draws the game's main UI elements, including ball status and current call.

    @param[in]     layout       Visual/layout positions for UI elements.
    @param[in]     balls        Ball pool state (remaining counts, etc.).
    @param[in]     currentCall  Current active call information to display.
*/
void bingo_drawUI(const Layout_St* const layout, const BallSystem_St* const balls, const CallState_St* const currentCall);

#endif // UI_APP_H