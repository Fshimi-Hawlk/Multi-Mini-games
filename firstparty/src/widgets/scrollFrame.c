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

#include "widgets/scrollFrame.h"
#include "widgets/types.h"

#include "utils/mathUtils.h"

// ─────────────────────────────────────────────────────────────────────────────

void scrollFrameInit(ScrollFrame_St* frame, f32 scrollSpeed) {
    frame->visibleHeight = 0.0f;
    frame->scrollY       = 0.0f;
    frame->contentHeight = 0.0f;
    frame->scrollSpeed   = (scrollSpeed > 0.0f) ? scrollSpeed : 60.0f;
}

bool scrollFrameUpdate(ScrollFrame_St* frame, Rectangle visibleArea, Vector2 mouseScreen) {
    if (!CheckCollisionPointRec(mouseScreen, visibleArea)) {
        return false;
    }

    f32 wheel = GetMouseWheelMove();
    if (wheel == 0.0f) {
        return false;
    }

    frame->scrollY -= wheel * frame->scrollSpeed;

    // Clamp scroll so we never show empty space when content is smaller than view
    f32 maxScroll = frame->contentHeight - frame->visibleHeight;
    if (maxScroll < 0.0f) maxScroll = 0.0f;   // content fits entirely

    frame->scrollY = clamp(frame->scrollY, 0.0f, maxScroll);

    return true;
}

void scrollFrameBegin(ScrollFrame_St* frame, Rectangle visibleArea) {
    frame->visibleHeight = visibleArea.height;

    BeginScissorMode((int)visibleArea.x,
                     (int)visibleArea.y,
                     (int)visibleArea.width,
                     (int)visibleArea.height);
}

void scrollFrameEnd(void) {
    EndScissorMode();
}