/**
    @file widgets/scrollFrame.c
    @author Fshimi-Hawlk
    @date 2026-03-28
    @brief Implementation of the reusable ScrollFrame widget.

    Contributors:
        - Fshimi-Hawlk:
            - Full reusable scroll frame with proper clamping
            - Clean integration point for all scrolling panels
*/

#include "sharedWidgets/scrollFrame.h"

#include "sharedUtils/mathUtils.h"

// ─────────────────────────────────────────────────────────────────────────────

void scrollFrameInit(ScrollFrame_St* frame, Rectangle visibleArea, f32Vector2 contentSize, f32 scrollSpeed, f32 roundness) {
    frame->visibleArea  = visibleArea;
    frame->scroll       = (f32Vector2) {0};
    frame->contentSize  = contentSize;
    frame->scrollSpeed  = (scrollSpeed > 0.0f) ? scrollSpeed : 60.0f;
    frame->roundness    = (roundness > 0.0f) ? roundness : 1.0f;
}

bool scrollFrameUpdate(ScrollFrame_St* frame, Vector2 mouseScreen) {
    if (!CheckCollisionPointRec(mouseScreen, frame->visibleArea)) {
        return false;
    }

    f32 wheel = GetMouseWheelMove();
    if (wheel == 0.0f) {
        return false;
    }

    frame->scroll.y -= wheel * frame->scrollSpeed;

    // Clamp scroll so we never show empty space when content is smaller than view
    f32 maxScroll = frame->contentSize.y - frame->visibleArea.height;
    if (maxScroll < 0.0f) maxScroll = 0.0f;   // content fits entirely

    frame->scroll.y = clamp(frame->scroll.y, 0.0f, maxScroll);

    return true;
}

void scrollFrameBegin(ScrollFrame_St* frame) {
    BeginScissorMode((int)frame->visibleArea.x,
                     (int)frame->visibleArea.y,
                     (int)frame->visibleArea.width,
                     (int)frame->visibleArea.height);
}

void scrollFrameEnd(void) {
    EndScissorMode();
}