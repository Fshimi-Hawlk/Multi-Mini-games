/**
    @file textBox.c
    @author Multi Mini-Games Team
    @date 2026-03-28
    @date 2026-04-13
    @brief Implementation of the single-line TextBox widget

    Contributors:
        - Fshimi-Hawlk:
            - Full mouse focus + typing support
            - Backspace, Delete, left/right arrow cursor movement
            - Blinking cursor when active
            - Proper state machine (normal/hover/active)
            - Added roundness, placeholder support and validation border coloring
            - Fixed character insertion at arbitrary cursor position (now properly shifts suffix right instead of overwriting with \0)
            - Fixed backspace/delete when cursor is in the middle of text (now shifts correctly)
            - Guaranteed buffer safety (never exceeds 255 chars, always null-terminated)
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

    size_t len = strlen(box->buffer);

    // ── Typing (proper insertion - shifts remaining text right) ────────────
    int key = GetCharPressed();
    while (key > 0) {
        if (key >= 32 && key <= 125 && len < 255) {  // printable ASCII, room available
            // Shift tail right (including the null terminator)
            memmove(box->buffer + box->cursorPos + 1,
                    box->buffer + box->cursorPos,
                    len - box->cursorPos + 1);
            box->buffer[box->cursorPos] = (char)key;
            box->cursorPos++;
            len++;
        }
        key = GetCharPressed();
    }

    // ── Backspace (now works anywhere - shifts left) ───────────────────────
    if (IsKeyPressed(KEY_BACKSPACE) || IsKeyPressedRepeat(KEY_BACKSPACE)) {
        if (box->cursorPos > 0) {
            memmove(box->buffer + box->cursorPos - 1,
                    box->buffer + box->cursorPos,
                    len - box->cursorPos + 1);
            box->cursorPos--;
            len--;
        }
    }

    // ── Delete ─────────────────────────────────────────────────────────────
    if (IsKeyPressed(KEY_DELETE) || IsKeyPressedRepeat(KEY_DELETE)) {
        if (box->cursorPos < len) {
            memmove(box->buffer + box->cursorPos,
                    box->buffer + box->cursorPos + 1,
                    len - box->cursorPos);
            len--;
        }
    }

    // ── Cursor movement ────────────────────────────────────────────────────
    if (IsKeyPressed(KEY_LEFT) || IsKeyPressedRepeat(KEY_LEFT)) {
        if (box->cursorPos > 0) box->cursorPos--;
    }
    if (IsKeyPressed(KEY_RIGHT) || IsKeyPressedRepeat(KEY_RIGHT)) {
        if (box->cursorPos < len) box->cursorPos++;
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

        box->cursorPos = (u32) clamp(clickX / (len > 0 ? len : 1), 0, len);
    }

    return committed;
}

void textBoxDraw(const TextBox_St* box, Font font, f32 fontSize) {
    bool isActive = (box->state == WIDGET_STATE_ACTIVE);
    bool isHovered = (box->state == WIDGET_STATE_HOVER);

    Color bgColor = isActive ? RAYWHITE : (isHovered ? Fade(LIGHTGRAY, 0.5f) : Fade(LIGHTGRAY, 0.3f));
    DrawRectangleRounded(box->bounds, box->roundness, 8, bgColor);

    Color borderColor = isActive ? (box->isValid ? (Color){0, 150, 255, 255} : RED) : (isHovered ? GRAY : DARKGRAY);
    float lineThick = isActive ? 2.0f : 1.0f;
    DrawRectangleRoundedLinesEx(box->bounds, box->roundness, 8, lineThick, borderColor);

    Vector2 textPos = {
        box->bounds.x + 12.0f,
        box->bounds.y + (box->bounds.height - fontSize) * 0.5f
    };

    if (box->buffer[0] != '\0') {
        DrawTextEx(font, box->buffer, textPos, fontSize, 0.0f, DARKGRAY);
    } else if (box->placeholder && !isActive) {
        DrawTextEx(font, box->placeholder, textPos, fontSize, 0.0f, GRAY);
    }

    if (isActive) {
        static f32 blinkTimer = 0.0f;
        blinkTimer += GetFrameTime();

        if (((int)(blinkTimer * 2.0f)) % 2 == 0) {
            char temp[256];
            strncpy(temp, box->buffer, box->cursorPos);
            temp[box->cursorPos] = '\0';

            Vector2 textSize = MeasureTextEx(font, temp, fontSize, 0.0f);
            Vector2 cursorPos = {
                textPos.x + textSize.x + 1.0f,
                textPos.y + 2.0f
            };

            DrawRectangle((int)cursorPos.x, (int)cursorPos.y, 2, (int)fontSize - 4, (Color){0, 150, 255, 255});
        }
        if (blinkTimer > 1.0f) blinkTimer = 0.0f;
    }
}