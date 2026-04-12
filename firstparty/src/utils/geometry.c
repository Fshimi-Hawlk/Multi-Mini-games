/**
    @file firstparty/src/geometry/geometry.c
    @author Fshimi-Hawlk
    @date 2026-04-03
    @date 2026-04-03
    @brief Implementation of rectangle anchoring and anchored text drawing.

    Contributors:
        - Fshimi-Hawlk:
            - Extracted from utils.c/utils.h

    @see geometry.h
*/

#include "sharedUtils/geometry.h"
#include "sharedUtils/debug.h"


Rectangle getAnchoredRect(Rectangle rect, Anchor_Et anchor) {
    switch (anchor) {
        case ANCHOR_TOP_LEFT: {
            return (Rectangle) {
                .x = rect.x,
                .y = rect.y,
                .width = rect.width,
                .height = rect.height
            };
        }

        case ANCHOR_TOP: {
            return (Rectangle) {
                .x = rect.x - rect.width / 2.0f,
                .y = rect.y,
                .width = rect.width,
                .height = rect.height
            };
        }

        case ANCHOR_TOP_RIGHT: {
            return (Rectangle) {
                .x = rect.x - rect.width,
                .y = rect.y,
                .width = rect.width,
                .height = rect.height
            };
        }

        case ANCHOR_LEFT: {
            return (Rectangle) {
                .x = rect.x,
                .y = rect.y - rect.height / 2.0f,
                .width = rect.width,
                .height = rect.height
            };
        }

        case ANCHOR_CENTER: {
            return (Rectangle) {
                .x = rect.x - rect.width / 2.0f,
                .y = rect.y - rect.height / 2.0f,
                .width = rect.width,
                .height = rect.height
            };
        }

        case ANCHOR_RIGHT: {
            return (Rectangle) {
                .x = rect.x - rect.width,
                .y = rect.y - rect.height / 2.0f,
                .width = rect.width,
                .height = rect.height
            };
        }

        case ANCHOR_BOTTOM_LEFT: {
            return (Rectangle) {
                .x = rect.x,
                .y = rect.y - rect.height,
                .width = rect.width,
                .height = rect.height
            };
        }

        case ANCHOR_BOTTOM: {
            return (Rectangle) {
                .x = rect.x - rect.width / 2.0f,
                .y = rect.y - rect.height,
                .width = rect.width,
                .height = rect.height
            };
        }

        case ANCHOR_BOTTOM_RIGHT: {
            return (Rectangle) {
                .x = rect.x - rect.width,
                .y = rect.y - rect.height,
                .width = rect.width,
                .height = rect.height
            };
        }

        default: {
            UNREACHABLE("Unknown Anchor_Et value");
            return rect; // unreachable
        }
    }
}

void drawTextPro(const char* text, Font font, f32Vector2 pos, Anchor_Et anchor, Color color) {
    f32Vector2 textSize = MeasureTextEx(font, text, font.baseSize, 0);
    f32Vector2 textPos = getRectPos(getAnchoredRect(createRect(pos, textSize), anchor));
    DrawTextEx(font, text, textPos, font.baseSize, 0, color);
}