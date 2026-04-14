/**
    @file prompt.h
    @author Kimi BERGE
    @date 2026-04-07
    @date 2026-04-14
    @brief Save/load modal UI using the widget system.
*/
#ifndef UI_PROMPT_H
#define UI_PROMPT_H

#include "utils/userTypes.h"

/**
    @brief Updates all active prompts and returns true if the window should close.

    @param[in,out] game         Pointer to the current game state.
    @param[in]     mouse        The current mouse position.
    @return                     true if the window should close, false otherwise.
*/
bool polyBlast_promptUpdate(GameState_St* const game, Vector2 mouse);

/**
    @brief Draws the active prompt (if any).
*/
void polyBlast_promptDraw(void);

#endif // UI_PROMPT_H