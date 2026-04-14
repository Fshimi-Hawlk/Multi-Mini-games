/**
    @file textBox.h
    @author Multi Mini-Games Team
    @date 2026-03-28
    @date 2026-04-14
    @brief Single-line text input widget.
*/
#ifndef WIDGETS_TEXT_BOX_H
#define WIDGETS_TEXT_BOX_H

#include "sharedWidgets/types.h"

/**
    @brief Updates a text box (handles typing, backspace, focus).
    @param box          Pointer to TextBox_St
    @param mouseScreen  Current mouse position
    @return true if the text content changed this frame
*/
bool textBoxUpdate(TextBox_St* box, Vector2 mouseScreen);

/**
    @brief Draws a text box with cursor blink when active.
    @param box          TextBox_St to draw
    @param font         Font to use
    @param fontSize     Font size
*/
void textBoxDraw(const TextBox_St* box, Font font, f32 fontSize);

#endif // WIDGETS_TEXT_BOX_H