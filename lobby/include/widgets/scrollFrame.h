/**
    @file widgets/scrollFrame.h
    @author Fshimi-Hawlk
    @date 2026-03-28
    @brief Container for scrollable content.
*/

#ifndef WIDGETS_SCROLL_FRAME_H
#define WIDGETS_SCROLL_FRAME_H

#include "widgets/types.h"

/**
    @brief Initializes a scroll frame.
    @param frame    Pointer to ScrollFrame_St
    @param bounds   Outer visible rectangle
    @param content  Inner total content rectangle
*/
void scrollFrameInit(ScrollFrame_St* frame, Rectangle bounds, Rectangle content);

/**
    @brief Updates scroll position based on mouse wheel.
    @param frame        Pointer to ScrollFrame_St
    @param mouseScreen  Current mouse position
    @return true if scrolled
*/
bool scrollFrameUpdate(ScrollFrame_St* frame, Vector2 mouseScreen);

/**
    @brief Starts clipping for the scroll frame.
*/
void scrollFrameBegin(ScrollFrame_St* frame);

/**
    @brief Ends clipping.
*/
void scrollFrameEnd(void);

#endif // WIDGETS_SCROLL_FRAME_H
