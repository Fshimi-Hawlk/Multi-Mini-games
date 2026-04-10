/**
    @file sharedWidgets/scrollFrame.h
    @author Fshimi-Hawlk
    @date 2026-03-28
    @brief Reusable scrollable frame – replaces all the repeated scrollY + scissor logic.
*/

#ifndef WIDGETS_SCROLL_FRAME_H
#define WIDGETS_SCROLL_FRAME_H

#include "sharedWidgets/types.h"

/**
    @brief Initializes a ScrollFrame_St with sensible defaults.
    @param frame         Pointer to the scroll frame
    @param scrollSpeed   Pixels per wheel unit (0 = use default 60.0f)
    @param visibleArea   Screen-space rectangle of the visible region
    @param contentHeight Total virtual height of the content
    @param roundness     Visible area rectangle roundness
*/
void scrollFrameInit(ScrollFrame_St* frame, Rectangle visibleArea, f32 contentHeight, f32 scrollSpeed, f32 roundness);

/**
    @brief Updates scroll offset from mouse wheel when mouse is inside the area.
           Automatically clamps scrollY.
    @param frame        Scroll frame to update
    @param mouseScreen  Current mouse position
    @return true if scrolling occurred this frame
*/
bool scrollFrameUpdate(ScrollFrame_St* frame, Vector2 mouseScreen);

/**
    @brief Begins scissor mode and applies scroll transform.
           Call this before drawing the scrollable content.
    @param frame        Scroll frame
*/
void scrollFrameBegin(ScrollFrame_St* frame);

/**
    @brief Ends the scissor mode. Must be paired with scrollFrameBegin().
*/
void scrollFrameEnd(void);

#endif // WIDGETS_SCROLL_FRAME_H