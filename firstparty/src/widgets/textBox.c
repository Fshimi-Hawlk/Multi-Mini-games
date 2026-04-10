/**
    @file widgets/textBox.c
    @author Fshimi-Hawlk
    @date 2026-03-28
    @date 2026-03-30
    @brief Implementation of the single-line TextBox widget

    Contributors:
        - Fshimi-Hawlk:
            - Full mouse focus + typing support
            - Backspace, Delete, left/right arrow cursor movement
            - Blinking cursor when active
            - Proper state machine (normal/hover/active)
            - Added roundness, placeholder support and validation border coloring
*/

#include <string.h>

#include "sharedWidgets/textBox.h"

#include "sharedUtils/mathUtils.h"

// ─────────────────────────────────────────────────────────────────────────────

bool textBoxUpdate(TextBox_St* box, Vector2 mouseScreen) {
    if (box->state == WIDGET_STATE_DISABLED) {
        return false;
    }

    bool hovered = CheckCollisionPointRec(mouseScreen, box->bounds);
    bool clicked = IsMouseButtonPressed(MOUSE_BUTTON_LEFT);

    // ── Focus handling ─────────────────────────────────────────────────────
    if (hovered) {
        if (clicked) {
            box->state = WIDGET_STATE_ACTIVE;
            box->editMode = true;
        } else if (box->state != WIDGET_STATE_ACTIVE) {
            box->state = WIDGET_STATE_HOVER;
        }
    } else {
        if (clicked) {
            box->state = WIDGET_STATE_NORMAL;
            if (box->editMode) {
                box->editMode = false;
            }
        } else if (box->state != WIDGET_STATE_ACTIVE) {
            box->state = WIDGET_STATE_NORMAL;
        }
    }

    if (!box->editMode) {
        return false;
    }

    bool committed = false;

    // ── Typing ─────────────────────────────────────────────────────────────
    int key = GetCharPressed();
    while (key > 0) {
        if ((key >= 32 && key <= 125) && box->cursorPos < 255) {  // printable ASCII
            box->buffer[box->cursorPos++] = (char)key;
            box->buffer[box->cursorPos] = '\0';
        }
        key = GetCharPressed();
    }

    // ── Backspace / Delete ─────────────────────────────────────────────────
    if (IsKeyPressed(KEY_BACKSPACE) || IsKeyPressedRepeat(KEY_BACKSPACE)) {
        if (box->cursorPos > 0) {
            box->cursorPos--;
            box->buffer[box->cursorPos] = '\0';
        }
    }

    if (IsKeyPressed(KEY_DELETE) || IsKeyPressedRepeat(KEY_DELETE)) {
        if (box->cursorPos < strlen(box->buffer)) {
            // shift remaining characters left
            for (u32 i = box->cursorPos; i < 255 && box->buffer[i + 1]; ++i) {
                box->buffer[i] = box->buffer[i + 1];
            }
            box->buffer[strlen(box->buffer) - 1] = '\0';
        }
    }

    // ── Cursor movement ────────────────────────────────────────────────────
    if (IsKeyPressed(KEY_LEFT) || IsKeyPressedRepeat(KEY_LEFT)) {
        if (box->cursorPos > 0) box->cursorPos--;
    }
    if (IsKeyPressed(KEY_RIGHT) || IsKeyPressedRepeat(KEY_RIGHT)) {
        if (box->cursorPos < strlen(box->buffer)) box->cursorPos++;
    }

    // ── Commit on Enter ────────────────────────────────────────────────────
    if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_KP_ENTER)) {
        committed = true;
        box->editMode = false;
        box->state = WIDGET_STATE_NORMAL;
    }

    // ── Click to move cursor (optional but nice) ───────────────────────────
    if (clicked && hovered) {
        // Simple cursor placement near click position (can be refined later)
        f32 clickX = mouseScreen.x - box->bounds.x - 8.0f;

        box->cursorPos = (u32) clamp(clickX / strlen(box->buffer), 0, strlen(box->buffer));
    }

    return committed;
}

void textBoxDraw(const TextBox_St* box, Font font, f32 fontSize) {
    // Background
    Color bgColor = (box->state == WIDGET_STATE_ACTIVE) ? Fade(WHITE, 0.95f)
                  : (box->state == WIDGET_STATE_HOVER) ? Fade(LIGHTGRAY, 0.9f)
                  : LIGHTGRAY;

    DrawRectangleRounded(box->bounds, box->roundness, 8, bgColor);

    // Border color logic
    Color borderColor = DARKGRAY;
    if (box->state == WIDGET_STATE_ACTIVE) {
        borderColor = box->isValid ? GREEN : RED;   // validation feedback
    } else if (box->state == WIDGET_STATE_HOVER) {
        borderColor = SKYBLUE;
    }

    DrawRectangleRoundedLinesEx(box->bounds, box->roundness, 8, 2.0f, borderColor);

    // Text or placeholder
    Vector2 textPos = {
        box->bounds.x + 8.0f,
        box->bounds.y + (box->bounds.height - fontSize) * 0.5f
    };

    if (box->buffer[0] != '\0') {
        DrawTextEx(font, box->buffer, textPos, fontSize, 0.0f, BLACK);
    } else if (box->placeholder && box->state != WIDGET_STATE_ACTIVE) {
        DrawTextEx(font, box->placeholder, textPos, fontSize, 0.0f, GRAY);
    }

    // Blinking cursor when active
    if (box->state == WIDGET_STATE_ACTIVE) {
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