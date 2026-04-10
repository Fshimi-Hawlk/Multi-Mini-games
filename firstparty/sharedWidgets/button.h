/**
    @file sharedWidgets/button.h
    @author Fshimi-Hawlk
    @date 2026-03-28
    @brief Text and image button widgets.
*/

#ifndef WIDGETS_BUTTON_H
#define WIDGETS_BUTTON_H

#include "sharedWidgets/types.h"

/**
    @brief Updates a text button and returns true if it was clicked this frame.
    @param btn          Pointer to TextButton_St
    @param mouseScreen  Current mouse position
    @return true if clicked
*/
bool textButtonUpdate(TextButton_St* btn, Vector2 mouseScreen);

/**
    @brief Draws a text button.
    @param btn          TextButton_St to draw
    @param font         Font to use
    @param fontSize     Font size
*/
void textButtonDraw(const TextButton_St* btn, Font font, f32 fontSize);

/**
    @brief Updates an image button and returns true if clicked.
    @param btn          Pointer to ImageButton_St
    @param mouseScreen  Current mouse position
    @return true if clicked
*/
bool imageButtonUpdate(ImageButton_St* btn, Vector2 mouseScreen);

/**
    @brief Draws an image button.
    @param btn          ImageButton_St to draw
    @param drawFrame    Do draw ImageButton_St's frame
*/
void imageButtonDraw(const ImageButton_St* btn, bool drawFrame);

#endif // WIDGETS_BUTTON_H