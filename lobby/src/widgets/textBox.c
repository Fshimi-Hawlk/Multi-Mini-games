/**
    @file widgets/textBox.c
    @author Fshimi-Hawlk
    @date 2026-03-28
    @brief Implementation of the single-line TextBox widget.
*/

#include "widgets/textBox.h"
#include "widgets/types.h"
#include "utils/utils.h"
#include <string.h>

bool textBoxUpdate(TextBox_St* box, Vector2 mouseScreen) {
    if (box->state == WIDGET_STATE_DISABLED) return false;

    bool hovered = CheckCollisionPointRec(mouseScreen, box->bounds);
    bool clicked = IsMouseButtonPressed(MOUSE_BUTTON_LEFT);

    if (hovered) {
        if (clicked) {
            box->state = WIDGET_STATE_CLICK;
            box->editMode = true;
        } else if (box->state != WIDGET_STATE_CLICK) {
            box->state = WIDGET_STATE_HOVERED;
        }
    } else {
        if (clicked) {
            box->state = WIDGET_STATE_NORMAL;
            box->editMode = false;
        } else if (box->state != WIDGET_STATE_CLICK) {
            box->state = WIDGET_STATE_NORMAL;
        }
    }

    if (!box->editMode) return false;

    bool changed = false;

    // Typing
    int key = GetCharPressed();
    while (key > 0) {
        if ((key >= 32 && key <= 125) && strlen(box->buffer) < 255) {
            // Shift others
            size_t len = strlen(box->buffer);
            for (size_t i = len; i > box->cursorPos; i--) {
                box->buffer[i] = box->buffer[i-1];
            }
            box->buffer[box->cursorPos++] = (char)key;
            box->buffer[len + 1] = '\0';
            changed = true;
        }
        key = GetCharPressed();
    }

    // Backspace
    if (IsKeyPressed(KEY_BACKSPACE) || IsKeyPressedRepeat(KEY_BACKSPACE)) {
        if (box->cursorPos > 0) {
            size_t len = strlen(box->buffer);
            for (size_t i = box->cursorPos - 1; i < len; i++) {
                box->buffer[i] = box->buffer[i+1];
            }
            box->cursorPos--;
            changed = true;
        }
    }

    // Delete
    if (IsKeyPressed(KEY_DELETE) || IsKeyPressedRepeat(KEY_DELETE)) {
        size_t len = strlen(box->buffer);
        if (box->cursorPos < len) {
            for (size_t i = box->cursorPos; i < len; i++) {
                box->buffer[i] = box->buffer[i+1];
            }
            changed = true;
        }
    }

    // Cursor movement
    if (IsKeyPressed(KEY_LEFT) || IsKeyPressedRepeat(KEY_LEFT)) {
        if (box->cursorPos > 0) box->cursorPos--;
    }
    if (IsKeyPressed(KEY_RIGHT) || IsKeyPressedRepeat(KEY_RIGHT)) {
        if (box->cursorPos < strlen(box->buffer)) box->cursorPos++;
    }

    // Commit on Enter
    if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_KP_ENTER)) {
        box->editMode = false;
        box->state = WIDGET_STATE_NORMAL;
    }

    return changed;
}

void textBoxDraw(const TextBox_St* box, Font font, f32 fontSize) {
    Color bgColor = (box->state == WIDGET_STATE_CLICK) ? Fade(WHITE, 0.95f)
                  : (box->state == WIDGET_STATE_HOVERED) ? Fade(LIGHTGRAY, 0.9f)
                  : LIGHTGRAY;

    DrawRectangleRounded(box->bounds, box->roundness, 8, bgColor);

    Color borderColor = DARKGRAY;
    if (box->state == WIDGET_STATE_CLICK) {
        borderColor = box->isValid ? GREEN : RED;
    } else if (box->state == WIDGET_STATE_HOVERED) {
        borderColor = SKYBLUE;
    }

    DrawRectangleRoundedLinesEx(box->bounds, box->roundness, 8, 2.0f, borderColor);

    Vector2 textPos = {
        box->bounds.x + 8.0f,
        box->bounds.y + (box->bounds.height - fontSize) * 0.5f
    };

    if (box->buffer[0] != '\0') {
        DrawTextEx(font, box->buffer, textPos, fontSize, 0.0f, BLACK);
    } else if (box->placeholder && box->state != WIDGET_STATE_CLICK) {
        DrawTextEx(font, box->placeholder, textPos, fontSize, 0.0f, GRAY);
    }

    if (box->state == WIDGET_STATE_CLICK) {
        static f32 blinkTimer = 0.0f;
        blinkTimer += GetFrameTime();

        if (((int)(blinkTimer * 2.0f)) % 2 == 0) {
            char temp[256];
            strncpy(temp, box->buffer, box->cursorPos);
            temp[box->cursorPos] = '\0';

            Vector2 textSize = MeasureTextEx(font, temp, fontSize, 0.0f);
            Vector2 cursorPos = {
                box->bounds.x + 8.0f + textSize.x,
                box->bounds.y + (box->bounds.height - fontSize) * 0.5f
            };

            DrawRectangle((int)cursorPos.x, (int)cursorPos.y, 2, (int)fontSize, BLACK);
        }
        if (blinkTimer > 1.0f) blinkTimer = 0.0f;
    }
}
