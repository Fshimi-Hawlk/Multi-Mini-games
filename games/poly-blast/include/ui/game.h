/**
    @file game.h
    @author Kimi BERGE
    @date 2026-01-07
    @date 2026-04-14
    @brief Main UI drawing entry point and text helpers.
*/
#ifndef UI_GAME_H
#define UI_GAME_H

#include "utils/userTypes.h"

/**
    @brief Calculates the position required to center text at a given point.

    @param[in]     text         The string to measure.
    @param[in]     font         The font to use for measurement.
    @param[in]     fontSize     The size of the font.
    @param[in]     pos          The target center point.
    @return                     The top-left position vector to draw the text.
*/
f32Vector2 polyBlast_getTextCenterPosition(const char* const text, Font font, f32 fontSize, f32Vector2 pos);

/**
    @brief Draws centered text at the specified position.

    @param[in]     text         The string to draw.
    @param[in]     font         The font to use.
    @param[in]     fontSize     The size of the font.
    @param[in]     pos          The target center point.
    @param[in]     tint         The color tint of the text.
*/
void polyBlast_drawText(const char* const text, Font font, f32 fontSize, f32Vector2 pos, Color tint);

/**
    @brief Draws all UI elements for the current scene.

    In GAME scene: renders the board and the three prefab slots (skipping placed ones).
    In ALL_PREFABS scene: renders every prefab in the bag (used for debugging/visualization).

    @param[in]     game         Pointer to the current game state.
*/
void polyBlast_drawUI(const PolyBlastGame_St* const game);

#endif // UI_GAME_H