/**
    @file widgets/dropdown.h
    @author Fshimi-Hawlk
    @date 2026-03-28
    @brief Dropdown widget for the reusable UI system.
*/

#ifndef WIDGETS_DROPDOWN_H
#define WIDGETS_DROPDOWN_H

#include "widgets/types.h"

/**
    @brief Updates a dropdown widget (handles open/close, selection, mouse interaction).
    @param dd           Pointer to DropDown_St – state and selectedIndex will be modified
    @param mouseScreen  Current mouse position in screen space
    @param optionCount  Number of options in the list (used to compute list height)
    @return true if the selected index changed this frame
*/
bool dropdownUpdate(DropDown_St* dd, Vector2 mouseScreen, u32 optionCount);

/**
    @brief Draws the dropdown button and the expanded list when open.
    @param dd           DropDown_St to render
    @param options      Array of option strings (must remain valid while dropdown is open)
    @param optionCount  Number of options in the array
    @param font         Font to use
    @param fontSize     Font size
*/
void dropdownDraw(const DropDown_St* dd, const char** options, u32 optionCount,
                  Font font, f32 fontSize);

#endif // WIDGETS_DROPDOWN_H