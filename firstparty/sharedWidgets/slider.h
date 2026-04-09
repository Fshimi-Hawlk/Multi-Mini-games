/**
    @file sharedWidgets/slider.h
    @author Fshimi-Hawlk
    @date 2026-03-28
    @brief Horizontal slider widget.
*/

#ifndef WIDGETS_SLIDER_H
#define WIDGETS_SLIDER_H

#include "sharedWidgets/types.h"

/**
    @brief Updates a slider and returns true if value changed.
    @param slider       Pointer to Slider_St
    @param mouseScreen  Current mouse position
    @return true if value was modified
*/
bool sliderUpdate(Slider_St* slider, Vector2 mouseScreen);

/**
    @brief Draws the slider with current value.
    @param slider       Slider_St to draw
    @param font         Font for value label
    @param fontSize     Font size
*/
void sliderDraw(const Slider_St* slider, Font font, f32 fontSize);

#endif // WIDGETS_SLIDER_H