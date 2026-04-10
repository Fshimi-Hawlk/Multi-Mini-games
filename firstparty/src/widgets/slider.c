/**
    @file widgets/slider.c
    @author Fshimi-Hawlk
    @date 2026-03-28
    @brief Implementation of the horizontal Slider widget.

    Contributors:
        - Fshimi-Hawlk:
            - Drag-to-change value with proper clamping
            - Visual feedback for hover/active states
            - Filled progress bar + circular knob
            - Optional value label
*/

#include <stdio.h>

#include "sharedWidgets/slider.h"

#include "sharedUtils/mathUtils.h"

// ─────────────────────────────────────────────────────────────────────────────

bool sliderUpdate(Slider_St* slider, Vector2 mouseScreen) {
    if (slider->state == WIDGET_STATE_DISABLED) {
        return false;
    }

    bool hovered = CheckCollisionPointRec(mouseScreen, slider->bounds);

    if (hovered) {
        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
            slider->state = WIDGET_STATE_ACTIVE;
        } else {
            slider->state = WIDGET_STATE_HOVER;
        }
    } else if (slider->state != WIDGET_STATE_ACTIVE) {
        slider->state = WIDGET_STATE_NORMAL;
    }

    if (slider->state == WIDGET_STATE_ACTIVE && IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
        f32 relativeX = mouseScreen.x - slider->bounds.x;
        f32 t = relativeX / slider->bounds.width;
        t = clamp(t, 0.0f, 1.0f);

        f32 oldValue = slider->value;
        slider->value = slider->minValue + t * (slider->maxValue - slider->minValue);

        return (fabs(slider->value - oldValue) > 0.0001f);
    }

    if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
        slider->state = hovered ? WIDGET_STATE_HOVER : WIDGET_STATE_NORMAL;
    }

    return false;
}

void sliderDraw(const Slider_St* slider, Font font, f32 fontSize) {
    // Background track
    DrawRectangleRec(slider->bounds, LIGHTGRAY);
    DrawRectangleLinesEx(slider->bounds, 2.0f, DARKGRAY);

    // Filled progress
    f32 range = slider->maxValue - slider->minValue;
    f32 t = (range > 0.0f) ? (slider->value - slider->minValue) / range : 0.0f;
    t = clamp(t, 0.0f, 1.0f);

    Rectangle filled = slider->bounds;
    filled.width = filled.width * t;

    Color fillColor = (slider->state == WIDGET_STATE_ACTIVE) ? SKYBLUE
                    : (slider->state == WIDGET_STATE_HOVER) ? Fade(SKYBLUE, 0.9f)
                    : BLUE;

    DrawRectangleRec(filled, fillColor);

    // Knob
    f32 knobRadius = slider->bounds.height * 0.6f;
    Vector2 knobCenter = {
        slider->bounds.x + filled.width,
        slider->bounds.y + slider->bounds.height * 0.5f
    };

    DrawCircleV(knobCenter, knobRadius, WHITE);
    DrawCircleLinesV(knobCenter, knobRadius, (slider->state == WIDGET_STATE_ACTIVE) ? YELLOW : DARKGRAY);

    // Optional value label on the right
    if (font.baseSize > 0) {
        char label[32];
        snprintf(label, sizeof(label), "%.2f", slider->value);

        Vector2 labelSize = MeasureTextEx(font, label, fontSize, 0.0f);
        Vector2 labelPos = {
            slider->bounds.x + slider->bounds.width + 12.0f,
            slider->bounds.y + (slider->bounds.height - labelSize.y) * 0.5f
        };

        DrawTextEx(font, label, labelPos, fontSize, 0.0f, DARKGRAY);
    }
}