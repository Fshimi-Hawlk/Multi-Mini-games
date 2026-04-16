/**
    @file checkBox.c
    @author Multi Mini-Games Team
    @date 2026-03-29
    @date 2026-04-14
    @brief Implementation of the CheckBox widget.
*/
#include "sharedWidgets/checkBox.h"

// ─────────────────────────────────────────────────────────────────────────────

bool checkBoxUpdate(CheckBox_St* cb, Vector2 mouseScreen) {
    if (cb->state == WIDGET_STATE_DISABLED) return false;

    bool hovered = CheckCollisionPointRec(mouseScreen, cb->bounds);
    bool changed = false;

    if (hovered) {
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            cb->state = WIDGET_STATE_CLICK;
            cb->checked = !cb->checked;
            changed = true;
        } else {
            cb->state = WIDGET_STATE_HOVER;
        }
    } else {
        cb->state = WIDGET_STATE_NORMAL;
    }

    return changed;
}

void checkBoxDraw(const CheckBox_St* cb, Font font, f32 fontSize) {
    // Box
    Color boxColor = (cb->state == WIDGET_STATE_HOVER) ? Fade(LIGHTGRAY, 0.9f)
                   : (cb->state == WIDGET_STATE_CLICK) ? Fade(LIGHTGRAY, 0.7f)
                   : LIGHTGRAY;

    DrawRectangleRec(cb->bounds, boxColor);
    DrawRectangleLinesEx(cb->bounds, 2.0f, DARKGRAY);

    // Check mark
    if (cb->checked) {
        DrawLineEx(
            (Vector2){cb->bounds.x + 4.0f, cb->bounds.y + cb->bounds.height * 0.5f},
            (Vector2){cb->bounds.x + cb->bounds.width * 0.5f, cb->bounds.y + cb->bounds.height - 4.0f},
            3.0f, BLACK);
        DrawLineEx(
            (Vector2){cb->bounds.x + cb->bounds.width * 0.5f, cb->bounds.y + cb->bounds.height - 4.0f},
            (Vector2){cb->bounds.x + cb->bounds.width - 4.0f, cb->bounds.y + 4.0f},
            3.0f, BLACK);
    }

    // Label
    if (cb->label) {
        Vector2 labelPos = {
            cb->bounds.x + cb->bounds.width + 8.0f,
            cb->bounds.y + (cb->bounds.height - fontSize) * 0.5f
        };
        DrawTextEx(font, cb->label, labelPos, fontSize, 0.0f, BLACK);
    }
}