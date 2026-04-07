/**
    @file widgets/dropdown.h
    @author Fshimi-Hawlk
    @date 2026-03-28
    @brief DropDown menu widget.
*/

#ifndef WIDGETS_DROPDOWN_H
#define WIDGETS_DROPDOWN_H

#include "widgets/types.h"

/**
    @brief Updates a dropdown menu.
    @param dd           Pointer to Dropdown_St
    @param mouseScreen  Current mouse position
    @return true if selection changed this frame
*/
bool dropdownUpdate(Dropdown_St* dd, Vector2 mouseScreen);

/**
    @brief Draws a dropdown menu.
    @param dd           Dropdown_St to draw
    @param font         Font to use
    @param fontSize     Font size
*/
void dropdownDraw(const Dropdown_St* dd, Font font, f32 fontSize);

#endif // WIDGETS_DROPDOWN_H
