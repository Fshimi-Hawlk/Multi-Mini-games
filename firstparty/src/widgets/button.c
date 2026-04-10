/**
    @file widgets/button.c
    @author Fshimi-Hawlk
    @date 2026-03-28
    @brief Implementation of text and image button widgets.

    Contributors:
        - Fshimi-Hawlk:
            - Clean state machine for hover/active/click/disabled
            - Consistent visual feedback
*/

#include "sharedWidgets/button.h"

// ─────────────────────────────────────────────────────────────────────────────

bool textButtonUpdate(TextButton_St* btn, Vector2 mouseScreen) {
    if (btn->state == WIDGET_STATE_DISABLED) {
        return false;
    }

    bool hovered = CheckCollisionPointRec(mouseScreen, btn->bounds);

    if (hovered) {
        if (btn->state == WIDGET_STATE_CLICK && IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
            btn->state = WIDGET_STATE_HOVER;
            return true;
        }

        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
            btn->state = WIDGET_STATE_CLICK;
        } else {
            btn->state = WIDGET_STATE_HOVER;
        }
    } else {
        btn->state = WIDGET_STATE_NORMAL;
    }

    return false;
}

void textButtonDraw(const TextButton_St* btn, Font font, f32 fontSize) {
    Color bgColor = btn->baseColor;
    Color textColor = btn->textColor;

    switch (btn->state) {
        case WIDGET_STATE_HOVER:  bgColor = Fade(btn->baseColor, 0.85f); break;
        case WIDGET_STATE_CLICK:  bgColor = Fade(btn->baseColor, 0.65f); break;
        case WIDGET_STATE_DISABLED: {
            bgColor = Fade(btn->baseColor, 0.33f);
            textColor = Fade(btn->textColor, 0.33f);
        } break;
        default: break;
    }

    DrawRectangleRounded(btn->bounds, btn->roundness, 8, bgColor);
    DrawRectangleRoundedLinesEx(
        btn->bounds, btn->roundness, 8, 2.0f,
        (btn->state == WIDGET_STATE_ACTIVE) ? YELLOW : DARKGRAY
    );

    if (btn->text && btn->text[0]) {
        Vector2 textSize = MeasureTextEx(font, btn->text, fontSize, 0.0f);
        Vector2 pos = {
            btn->bounds.x + (btn->bounds.width - textSize.x) * 0.5f,
            btn->bounds.y + (btn->bounds.height - textSize.y) * 0.5f
        };
        DrawTextEx(font, btn->text, pos, fontSize, 0.0f, textColor);
    }
}

// ─────────────────────────────────────────────────────────────────────────────

bool imageButtonUpdate(ImageButton_St* btn, Vector2 mouseScreen) {
    if (btn->state == WIDGET_STATE_DISABLED) {
        return false;
    }

    bool hovered = CheckCollisionPointRec(mouseScreen, btn->bounds);

    if (hovered) {
        if (btn->state == WIDGET_STATE_CLICK && IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
            btn->state = WIDGET_STATE_HOVER;
            return true;
        }

        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
            btn->state = WIDGET_STATE_CLICK;
        } else {
            btn->state = WIDGET_STATE_HOVER;
        }
    } else {
        btn->state = WIDGET_STATE_NORMAL;
    }

    return false;
}

void imageButtonDraw(const ImageButton_St* btn, bool drawFrame) {
    Color tint = btn->tint;
    Color bgColor = btn->baseColor;

    switch (btn->state) {
        case WIDGET_STATE_HOVER: {
            tint = Fade(btn->tint, 0.9f); 
        } break;

        case WIDGET_STATE_CLICK: {
            tint = Fade(btn->tint, 0.7f);
        } break;
        case WIDGET_STATE_DISABLED: {
            tint = Fade(btn->tint, 0.5f);
        } break;

        default: break;
    }

    if (drawFrame) {
        DrawRectangleRounded(btn->bounds, btn->roundness, 8, bgColor);
        DrawRectangleRoundedLinesEx(
            btn->bounds, btn->roundness, 8, 2.0f,
            (btn->state == WIDGET_STATE_ACTIVE) ? YELLOW : DARKGRAY
        );
    }

    DrawTexturePro(
        btn->texture,
        (Rectangle){0, 0, (f32)btn->texture.width, (f32)btn->texture.height},
        btn->bounds,
        (Vector2){0},
        0.0f,
        tint
    );

    if (btn->state == WIDGET_STATE_HOVER || btn->state == WIDGET_STATE_CLICK) {
        DrawRectangleLinesEx(btn->bounds, 2.0f, YELLOW);
    }
}