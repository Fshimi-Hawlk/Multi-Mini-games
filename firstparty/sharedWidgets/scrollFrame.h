/**
    @file scrollFrame.h
    @author Multi Mini-Games Team
    @date 2026-03-28
    @date 2026-04-14
    @brief Reusable scrollable frame – replaces all the repeated scrollY + scissor logic.
*/
#ifndef WIDGETS_SCROLL_FRAME_H
#define WIDGETS_SCROLL_FRAME_H

#include "sharedWidgets/types.h"

/**
    @brief Initializes a ScrollFrame_St with sensible defaults.
    @param frame         Pointer to the scroll frame
    @param visibleArea   Screen-space rectangle of the visible region
    @param contentSize   Total virtual size of the content
    @param scrollSpeed   Pixels per wheel unit (0 = use default 60.0f)
    @param roundness     Visible area rectangle roundness
*/
void scrollFrameInit(ScrollFrame_St* frame, Rectangle visibleArea, f32Vector2 contentSize, f32 scrollSpeed, f32 roundness);

/**
    @brief Updates scroll offset from mouse wheel when mouse is inside the area.
           Automatically clamps scrollY.
    @param frame        Scroll frame to update
    @param mouseScreen  Current mouse position
    @return true if scrolling occurred this frame
*/
bool scrollFrameUpdate(ScrollFrame_St* frame, Rectangle visibleArea, Vector2 mouseScreen);

/**
    @brief Begins scissor mode and applies scroll transform.
           Call this before drawing the scrollable content.
    @param frame        Scroll frame
*/
void scrollFrameBegin(const ScrollFrame_St* frame);

/**
    @brief Ends the scissor mode. Must be paired with scrollFrameBegin().
*/
void scrollFrameEnd(void);

#endif // WIDGETS_SCROLL_FRAME_H