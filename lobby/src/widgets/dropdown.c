/**
    @file widgets/dropdown.c
    @author Fshimi-Hawlk
    @date 2026-03-28
    @brief Implementation of the DropDown widget.

    Contributors:
        - Fshimi-Hawlk:
            - Click to open/close
            - Mouse selection from expanded list
            - Visual feedback for hover/active states
            - Proper list bounds and outside-click close
            - Arrow that points down/up
*/

#include "widgets/dropdown.h"
#include "utils/common.h"
#include "utils/utils.h"
#include "widgets/types.h"

#include "systemSettings.h"

// ─────────────────────────────────────────────────────────────────────────────

bool dropdownUpdate(DropDown_St* dd, Vector2 mouseScreen, u32 optionCount) {
    if (dd->state == WIDGET_STATE_DISABLED) {
        return false;
    }

    bool changed = false;
    bool hovered = CheckCollisionPointRec(mouseScreen, dd->bounds);

    // Main button interaction
    if (hovered) {
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            dd->isOpen = !dd->isOpen;
            dd->state = WIDGET_STATE_ACTIVE;
        } else {
            dd->state = WIDGET_STATE_HOVER;
        }
    } else if (!dd->isOpen) {
        dd->state = WIDGET_STATE_NORMAL;
    }

    if (!dd->isOpen) {
        return false;
    }

    // When open: check clicks on the list items
    Rectangle listBounds = dd->bounds;
    listBounds.y += dd->bounds.height;
    listBounds.height = (f32)optionCount * 28.0f;   // 28px per item

    if (CheckCollisionPointRec(mouseScreen, listBounds)) {
        s32 hoveredIndex = (s32)((mouseScreen.y - listBounds.y) / 28.0f);

        if (hoveredIndex >= 0 && (u32)hoveredIndex < optionCount) {
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                if (dd->selectedIndex != hoveredIndex) {
                    dd->selectedIndex = hoveredIndex;
                    changed = true;
                }
                dd->isOpen = false;
                dd->state = WIDGET_STATE_NORMAL;
            }
        }
    } else if (!hovered && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        // Clicked outside the list → close
        dd->isOpen = false;
        dd->state = WIDGET_STATE_NORMAL;
    }

    return changed;
}

void dropdownDraw(const DropDown_St* dd, const char** options, u32 optionCount, Font font, f32 fontSize) {
    // ── Main button ────────────────────────────────────────────────────────
    Color bgColor = (dd->state == WIDGET_STATE_ACTIVE || dd->isOpen) ? Fade(SKYBLUE, 0.95f)
                  : (dd->state == WIDGET_STATE_HOVER) ? Fade(LIGHTGRAY, 0.9f)
                  : LIGHTGRAY;

    DrawRectangleRec(dd->bounds, bgColor);
    DrawRectangleLinesEx(dd->bounds, 2.0f, (dd->isOpen) ? YELLOW : DARKGRAY);

    // Selected text
    if (dd->selectedIndex >= 0 && (u32)dd->selectedIndex < optionCount) {
        const char* text = options[dd->selectedIndex];
        Vector2 textSize = MeasureTextEx(font, text, fontSize, 0.0f);
        Vector2 pos = {
            dd->bounds.x + 8.0f,
            dd->bounds.y + (dd->bounds.height - textSize.y) * 0.5f
        };
        DrawTextEx(font, text, pos, fontSize, 0.0f, BLACK);
    } else {
        DrawTextEx(font, "(none)", 
                   (Vector2) {dd->bounds.x + 8.0f, dd->bounds.y + (dd->bounds.height - fontSize) * 0.5f},
                   fontSize, 0.0f, GRAY);
    }

    
    // Arrow indicator
    Vector2 arrowPos = {
        dd->bounds.x + dd->bounds.width - 20.0f,
        dd->bounds.y + dd->bounds.height * 0.5f
    };

    DrawTriangle(
        (Vector2) {arrowPos.x, arrowPos.y - 6.0f},
        (Vector2) {arrowPos.x + 12.0f, arrowPos.y - 6.0f},
        (Vector2) {arrowPos.x + 6.0f, arrowPos.y + 6.0f},
        BLACK
    );

    // ── Expanded list ───────────────────────────────────────────────────────
    if (!dd->isOpen) return;

    Rectangle listRect = dd->bounds;
    listRect.y += dd->bounds.height;
    listRect.height = (f32)optionCount * 28.0f;

    DrawRectangleRec(listRect, Fade(WHITE, 0.98f));
    DrawRectangleLinesEx(listRect, 2.0f, SKYBLUE);

    for (u32 i = 0; i < optionCount; ++i) {
        Rectangle itemRect = {
            listRect.x,
            listRect.y + (f32)i * 28.0f,
            listRect.width,
            28.0f
        };

        bool itemHovered = CheckCollisionPointRec(GetMousePosition(), itemRect);

        Color itemColor = (i == (u32)dd->selectedIndex) ? Fade(SKYBLUE, 0.3f)
                        : (itemHovered) ? Fade(LIGHTGRAY, 0.7f)
                        : WHITE;

        DrawRectangleRec(itemRect, itemColor);

        if (i < optionCount - 1) {
            DrawLine((int)itemRect.x, (int)(itemRect.y + 28.0f),
                     (int)(itemRect.x + itemRect.width), (int)(itemRect.y + 28.0f),
                     LIGHTGRAY);
        }

        Vector2 textPos = {
            itemRect.x + 8.0f,
            itemRect.y + (28.0f - fontSize) * 0.5f
        };
        DrawTextEx(font, options[i], textPos, fontSize, 0.0f, BLACK);
    }
}