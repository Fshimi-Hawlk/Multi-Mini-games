/**
    @file widgets/scrollFrame.c
    @author Fshimi-Hawlk
    @date 2026-03-28
    @brief Implementation of the reusable ScrollFrame widget.
*/

#include "widgets/scrollFrame.h"
#include "widgets/types.h"
#include "utils/utils.h"

void scrollFrameInit(ScrollFrame_St* frame, Rectangle bounds, Rectangle content) {
    frame->bounds = bounds;
    frame->contentRect = content;
    frame->scroll = Vector2Zero();
}

bool scrollFrameUpdate(ScrollFrame_St* frame, Vector2 mouseScreen) {
    if (!CheckCollisionPointRec(mouseScreen, frame->bounds)) return false;

    float wheel = GetMouseWheelMove();
    if (wheel == 0.0f) return false;

    frame->scroll.y -= wheel * 30.0f;

    float maxScroll = frame->contentRect.height - frame->bounds.height;
    if (maxScroll < 0) maxScroll = 0;
    frame->scroll.y = clamp(frame->scroll.y, 0.0f, maxScroll);

    return true;
}

void scrollFrameBegin(ScrollFrame_St* frame) {
    BeginScissorMode((int)frame->bounds.x, (int)frame->bounds.y, (int)frame->bounds.width, (int)frame->bounds.height);
}

void scrollFrameEnd(void) {
    EndScissorMode();
}
