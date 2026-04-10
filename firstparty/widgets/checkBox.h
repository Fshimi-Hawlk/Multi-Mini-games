/**
    @file widgets/checkBox.h
    @author Fshimi-Hawlk
    @date 2026-03-29
    @brief Checkbox widget for the reusable UI system.
*/

#ifndef WIDGETS_CHECK_BOX_H
#define WIDGETS_CHECK_BOX_H

#include "widgets/types.h"

/**
    @brief Updates a checkbox and returns true if its checked state changed.
    @param cb           Pointer to CheckBox_St
    @param mouseScreen  Current mouse position
    @return true if toggled this frame
*/
bool checkBoxUpdate(CheckBox_St* cb, Vector2 mouseScreen);

/**
    @brief Draws a checkbox with optional label.
    @param cb           CheckBox_St to draw
    @param font         Font to use
    @param fontSize     Font size
*/
void checkBoxDraw(const CheckBox_St* cb, Font font, f32 fontSize);

#endif // WIDGETS_CHECK_BOX_H