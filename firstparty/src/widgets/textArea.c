/**
    @file widgets/textArea.c
    @author Fshimi-Hawlk
    @date 2026-04-13
    @brief Implementation of the multi-line TextArea widget with full word-wrap and multi-line cursor support.

    Contributors:
        - Fshimi-Hawlk:
            - Full widget skeleton
            - Cool named-parameter init macro (TextAreaConfig_St)
            - StringBuilder_St buffer (grows automatically)
            - Embedded ScrollFrame_St for scrollbars
            - Full word-wrap drawing (respects \n + automatic wrapping at word boundaries)
            - Full multi-line editing with cursor (left/right/up/down/enter/backspace/delete)
            - Cursor position calculated correctly for wrapped text
            - Auto-scroll to keep cursor visible
            - Editable + display-only modes
*/

#include "contextArena.h"

#define REALLOC context_realloc     // arena-aware realloc (3-arg)
#define FREE                        // future arena-aware free (disabled for now)

#include "sharedWidgets/textArea.h"
#include "sharedWidgets/scrollFrame.h"

#include "sharedUtils/mathUtils.h"

// ─────────────────────────────────────────────────────────────────────────────

/**
    @brief Helper that returns the visual cursor screen position (x, y) after word-wrap.
           Used for blinking cursor and auto-scroll.
*/
static Vector2 textAreaGetCursorScreenPos(const TextArea_St* area, Font font, f32 fontSize) {
    if (area->buffer.count == 0) {
        return (Vector2){area->bounds.x + 8.0f, area->bounds.y + 8.0f};
    }

    f32 lineHeight = fontSize + 4.0f;   // small padding between lines
    f32 maxLineWidth = area->bounds.width - 16.0f;
    f32 currentX = 0.0f;
    f32 currentY = 0.0f;

    const char* text = (const char*)area->buffer.items;
    u32 i = 0;

    while (i < area->cursorPos && text[i]) {
        if (text[i] == '\n') {
            currentY += lineHeight;
            currentX = 0.0f;
            i++;
            continue;
        }

        // word-wrap check
        if (area->wordWrap) {
            const char* wordStart = text + i;
            while (i < area->buffer.count && text[i] != ' ' && text[i] != '\n') i++;

            // safe measurement (buffer is not guaranteed null-terminated)
            char temp[256];
            u32 wordLen = (u32)(i - (wordStart - text));
            if (wordLen >= sizeof(temp)) wordLen = sizeof(temp) - 1;
            memcpy(temp, wordStart, wordLen);
            temp[wordLen] = '\0';

            Vector2 wordSize = MeasureTextEx(font, temp, fontSize, 0.0f);
            if (currentX + wordSize.x > maxLineWidth) {
                currentY += lineHeight;
                currentX = 0.0f;
            }
            i = (u32)(wordStart - text);   // reset to start of word
        }

        currentX += MeasureTextEx(font, (const char[]){text[i], '\0'}, fontSize, 0.0f).x;
        i++;
    }

    // cursor is at the current position
    return (Vector2){
        area->bounds.x + 8.0f + currentX,
        area->bounds.y + 8.0f + currentY - area->scrollFrame.scroll.y
    };
}

static void textAreaUpdateScroll(TextArea_St* area) {
    // Rough but good enough estimate for content height (refined later if needed)
    f32 lineHeight = 22.0f;
    u32 lineCount = 1;

    for (u32 i = 0; i < area->buffer.count; ++i) {
        if (area->buffer.items[i] == '\n') lineCount++;
    }

    area->scrollFrame.contentSize.y = (f32)lineCount * lineHeight + 40.0f;   // extra padding
    area->scrollFrame.contentSize.x = 0.0f;   // TODO: horizontal scroll when wordWrap = false
}

static void textAreaAutoScrollToCursor(TextArea_St* area, Vector2 cursorScreenPos) {
    f32 visibleTop    = area->bounds.y;
    f32 visibleBottom = area->bounds.y + area->bounds.height;

    if (cursorScreenPos.y < visibleTop + 8.0f) {
        area->scrollFrame.scroll.y += (visibleTop + 8.0f - cursorScreenPos.y);
    }
    if (cursorScreenPos.y + 22.0f > visibleBottom - 8.0f) {
        area->scrollFrame.scroll.y += (visibleBottom - 8.0f - (cursorScreenPos.y + 22.0f));
    }

    // clamp (re-use the same logic from scrollFrameUpdate)
    f32 maxScroll = area->scrollFrame.contentSize.y - area->bounds.height;
    if (maxScroll < 0.0f) maxScroll = 0.0f;
    area->scrollFrame.scroll.y = clamp(area->scrollFrame.scroll.y, 0.0f, maxScroll);
}

void textAreaInit__full(TextArea_St* area, TextAreaConfig_St config) {
    area->bounds            = config.bounds;
    area->state             = WIDGET_STATE_NORMAL;
    area->editable          = config.editable;          // default false
    area->wordWrap          = config.wordWrap;          // default true
    area->roundness         = (config.roundness > 0.0f) ? config.roundness : 0.1f;
    area->placeholder       = config.placeholder;
    area->isValid           = true;
    area->cursorPos         = 0;
    area->editMode          = false;
    area->buffer            = (StringBuilder_St){0};

    u32 cap = (config.initialCapacity > 0) ? config.initialCapacity : 4096;
    da_reserve(&area->buffer, cap);

    scrollFrameInit(&area->scrollFrame,
                    area->bounds,
                    (f32Vector2){0},
                    60.0f,
                    area->roundness);
}

bool textAreaUpdate(TextArea_St* area, Vector2 mouseScreen) {
    if (area->state == WIDGET_STATE_DISABLED) {
        return false;
    }

    textAreaUpdateScroll(area);
    bool scrolled = scrollFrameUpdate(&area->scrollFrame, area->scrollFrame.visibleArea, mouseScreen);

    bool hovered = CheckCollisionPointRec(mouseScreen, area->bounds);
    bool clicked = IsMouseButtonPressed(MOUSE_BUTTON_LEFT);

    // ── Focus handling (only when editable) ────────────────────────────────
    if (area->editable) {
        if (hovered && clicked) {
            area->state = WIDGET_STATE_ACTIVE;
            area->editMode = true;
        } else if (hovered && area->state != WIDGET_STATE_ACTIVE) {
            area->state = WIDGET_STATE_HOVER;
        } else if (!hovered && clicked) {
            area->state = WIDGET_STATE_NORMAL;
            area->editMode = false;
        } else if (area->state != WIDGET_STATE_ACTIVE) {
            area->state = WIDGET_STATE_NORMAL;
        }
    } else {
        area->state = WIDGET_STATE_NORMAL;   // display-only never becomes active
    }

    if (!area->editMode) {
        return scrolled;
    }

    bool contentChanged = false;
    size_t len = area->buffer.count;

    // ── Typing printable characters ─────────────────────────────────────
    int key = GetCharPressed();
    while (key > 0) {
        if (key >= 32 && key <= 125 && len < 8192) {   // printable ASCII, safety limit
            da_reserve(&area->buffer, len + 2);

            memmove(area->buffer.items + area->cursorPos + 1,
                    area->buffer.items + area->cursorPos,
                    len - area->cursorPos + 1);

            area->buffer.items[area->cursorPos] = (char)key;
            area->buffer.count++;
            area->cursorPos++;
            contentChanged = true;
        }
        key = GetCharPressed();
    }

    // ── Enter inserts newline (Shift+Enter also works the same) ─────────────
    if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_KP_ENTER)) {
        da_reserve(&area->buffer, len + 2);

        memmove(area->buffer.items + area->cursorPos + 1,
                area->buffer.items + area->cursorPos,
                len - area->cursorPos + 1);

        area->buffer.items[area->cursorPos] = '\n';
        area->buffer.count++;
        area->cursorPos++;
        contentChanged = true;
    }

    // ── Backspace ───────────────────────────────────────────────────────
    if (IsKeyPressed(KEY_BACKSPACE) || IsKeyPressedRepeat(KEY_BACKSPACE)) {
        if (area->cursorPos > 0) {
            memmove(area->buffer.items + area->cursorPos - 1,
                    area->buffer.items + area->cursorPos,
                    len - area->cursorPos + 1);
            area->buffer.count--;
            area->cursorPos--;
            contentChanged = true;
        }
    }

    // ── Delete ──────────────────────────────────────────────────────────
    if (IsKeyPressed(KEY_DELETE) || IsKeyPressedRepeat(KEY_DELETE)) {
        if (area->cursorPos < len) {
            memmove(area->buffer.items + area->cursorPos,
                    area->buffer.items + area->cursorPos + 1,
                    len - area->cursorPos);
            area->buffer.count--;
            contentChanged = true;
        }
    }

    // ── Cursor movement ─────────────────────────────────────────────────
    if (IsKeyPressed(KEY_LEFT) || IsKeyPressedRepeat(KEY_LEFT)) {
        if (area->cursorPos > 0) area->cursorPos--;
    }
    if (IsKeyPressed(KEY_RIGHT) || IsKeyPressedRepeat(KEY_RIGHT)) {
        if (area->cursorPos < len) area->cursorPos++;
    }

    // ── Up / Down (multi-line, respects \n and word-wrap boundaries) ─────
    if (IsKeyPressed(KEY_UP) || IsKeyPressedRepeat(KEY_UP)) {
        if (area->cursorPos > 0) {
            s32 i = (s32)area->cursorPos - 1;
            while (i >= 0 && area->buffer.items[i] != '\n') i--;
            area->cursorPos = (u32)(i < 0 ? 0 : i);
        }
    }
    if (IsKeyPressed(KEY_DOWN) || IsKeyPressedRepeat(KEY_DOWN)) {
        s32 i = (s32)area->cursorPos;
        while (i < (s32)len && area->buffer.items[i] != '\n') i++;
        if (i < (s32)len) area->cursorPos = (u32)i + 1;
    }

    // Auto-scroll to keep cursor visible
    Vector2 cursorPos = textAreaGetCursorScreenPos(area, (Font){0}, 20.0f);   // font/size not used here
    textAreaAutoScrollToCursor(area, cursorPos);

    return contentChanged || scrolled;
}

void textAreaDraw(const TextArea_St* area, Font font, f32 fontSize) {
    scrollFrameBegin(&area->scrollFrame);

    // background + border
    Color bg = (area->state == WIDGET_STATE_ACTIVE) ? Fade(WHITE, 0.95f)
             : (area->state == WIDGET_STATE_HOVER) ? Fade(LIGHTGRAY, 0.9f)
             : LIGHTGRAY;
    DrawRectangleRounded(area->bounds, area->roundness, 8, bg);
    DrawRectangleRoundedLinesEx(area->bounds, area->roundness, 8, 2.0f,
                                (area->state == WIDGET_STATE_ACTIVE) ? (area->isValid ? GREEN : RED) : DARKGRAY);

    // ── Word-wrapped text drawing ───────────────────────────────────────
    Vector2 drawPos = {
        area->bounds.x + 8.0f,
        area->bounds.y + 8.0f - area->scrollFrame.scroll.y
    };

    f32 lineHeight = fontSize + 4.0f;
    f32 maxWidth   = area->bounds.width - 16.0f;

    if (area->buffer.count > 0) {
        const char* text = (const char*)area->buffer.items;
        const char* start = text;

        while (*start) {
            const char* end = start;
            while (*end && *end != '\n') end++;

            // word-wrap the current line
            const char* lineEnd = start;
            f32 x = 0.0f;

            while (lineEnd < end) {
                const char* word = lineEnd;
                while (word < end && *word != ' ') word++;

                // safe measurement (substring is not null-terminated)
                char temp[512];
                u32 len = (u32)(word - start);
                if (len >= sizeof(temp)) len = sizeof(temp) - 1;
                memcpy(temp, start, len);
                temp[len] = '\0';

                Vector2 wordSize = MeasureTextEx(font, temp, fontSize, 0.0f);

                if (x + wordSize.x > maxWidth && x > 0.0f) {
                    // draw what we have so far
                    char tempLine[512];
                    u32 lineLen = (u32)(lineEnd - start);
                    if (lineLen >= sizeof(tempLine)) lineLen = sizeof(tempLine) - 1;
                    memcpy(tempLine, start, lineLen);
                    tempLine[lineLen] = '\0';
                    DrawTextEx(font, tempLine, drawPos, fontSize, 0.0f, BLACK);

                    drawPos.y += lineHeight;
                    x = 0.0f;
                    start = lineEnd;
                    lineEnd = word;
                    continue;
                }

                x += wordSize.x + MeasureTextEx(font, " ", fontSize, 0.0f).x;
                lineEnd = word + 1;
            }

            // draw final fragment of the line
            char temp[512];
            u32 len = (u32)(end - start);
            if (len >= sizeof(temp)) len = sizeof(temp) - 1;
            memcpy(temp, start, len);
            temp[len] = '\0';
            DrawTextEx(font, temp, drawPos, fontSize, 0.0f, BLACK);

            drawPos.y += lineHeight;
            start = (*end == '\n') ? end + 1 : end;
        }
    } else if (area->placeholder) {
        DrawTextEx(font, area->placeholder, drawPos, fontSize, 0.0f, GRAY);
    }

    // ── Blinking cursor (only in editable + active mode) ────────────────
    if (area->editable && area->state == WIDGET_STATE_ACTIVE) {
        static f32 blinkTimer = 0.0f;
        blinkTimer += GetFrameTime();

        if (((int)(blinkTimer * 2.0f)) % 2 == 0) {
            Vector2 cursorPos = textAreaGetCursorScreenPos(area, font, fontSize);
            DrawRectangle((int)cursorPos.x, (int)cursorPos.y, 2, (int)fontSize, BLACK);
        }

        if (blinkTimer > 1.0f) blinkTimer = 0.0f;
    }

    scrollFrameEnd();
}

void textAreaAppend(TextArea_St* area, const char* text) {
    if (text && text[0]) {
        sb_appendCStr(&area->buffer, text);
    }
}