/**
    @file widgets/button.c
    @author Fshimi-Hawlk
    @date 2026-03-28
    @brief Implementation of text and image button widgets.
*/

#include "widgets/button.h"
#include "utils/common.h"
#include "widgets/types.h"

bool textButtonUpdate(TextButton_St* btn, Vector2 mouseScreen) {
    if (btn->state == WIDGET_STATE_DISABLED) return false;

    bool hovered = CheckCollisionPointRec(mouseScreen, btn->bounds);

    if (hovered) {
        if (btn->state == WIDGET_STATE_CLICK && IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
            btn->state = WIDGET_STATE_HOVERED;
            return true;
        }

        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
            btn->state = WIDGET_STATE_CLICK;
        } else {
            btn->state = WIDGET_STATE_HOVERED;
        }
    } else {
        btn->state = WIDGET_STATE_NORMAL;
    }

    return false;
}

void textButtonDraw(const TextButton_St* btn, Font font, f32 fontSize) {
    Color bgColor = btn->baseColor;
    Color textColor = WHITE;

    switch (btn->state) {
        case WIDGET_STATE_HOVERED: bgColor = Fade(btn->baseColor, 0.85f); break;
        case WIDGET_STATE_CLICK:   bgColor = Fade(btn->baseColor, 0.65f); break;
        case WIDGET_STATE_DISABLED: {
            bgColor = (Color){70, 70, 80, 255};
            textColor = (Color){140, 140, 150, 255};
        } break;
        default: bgColor = Fade(btn->baseColor, 0.95f); break;
    }

    DrawRectangleRounded(btn->bounds, btn->roundness, 8, bgColor);
    DrawRectangleRoundedLinesEx(btn->bounds, btn->roundness, 8, 2.0f,
        (btn->state == WIDGET_STATE_CLICK) ? YELLOW : DARKGRAY);

    if (btn->text && btn->text[0]) {
        Vector2 textSize = MeasureTextEx(font, btn->text, fontSize, 0.0f);
        Vector2 pos = {
            btn->bounds.x + (btn->bounds.width - textSize.x) * 0.5f,
            btn->bounds.y + (btn->bounds.height - textSize.y) * 0.5f
        };
        DrawTextEx(font, btn->text, pos, fontSize, 0.0f, textColor);
    }
}

bool imageButtonUpdate(ImageButton_St* btn, Vector2 mouseScreen) {
    if (btn->state == WIDGET_STATE_DISABLED) return false;

    bool hovered = CheckCollisionPointRec(mouseScreen, btn->bounds);

    if (hovered) {
        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
            btn->state = WIDGET_STATE_CLICK;
        } else {
            btn->state = WIDGET_STATE_HOVERED;
        }
    } else {
        btn->state = WIDGET_STATE_NORMAL;
    }

    if (btn->state == WIDGET_STATE_CLICK && IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
        btn->state = WIDGET_STATE_HOVERED;
        return true;
    }

    return false;
}

void imageButtonDraw(const ImageButton_St* btn) {
    Color tint = btn->baseColor;

    switch (btn->state) {
        case WIDGET_STATE_HOVERED:  tint = Fade(btn->baseColor, 0.9f); break;
        case WIDGET_STATE_CLICK:    tint = Fade(btn->baseColor, 0.7f); break;
        case WIDGET_STATE_DISABLED: tint = Fade(btn->baseColor, 0.5f); break;
        default: break;
    }

    DrawTexturePro(
        btn->texture,
        (Rectangle){0, 0, (f32)btn->texture.width, (f32)btn->texture.height},
        btn->bounds,
        (Vector2){0},
        0.0f,
        tint
    );

    if (btn->state == WIDGET_STATE_HOVERED || btn->state == WIDGET_STATE_CLICK) {
        DrawRectangleLinesEx(btn->bounds, 2.0f, YELLOW);
    }
}
