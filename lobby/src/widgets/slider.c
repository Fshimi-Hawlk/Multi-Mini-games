/**
    @file widgets/slider.c
    @author Fshimi-Hawlk
    @date 2026-03-28
    @brief Implementation of the horizontal Slider widget.
*/

#include "widgets/slider.h"
#include "widgets/types.h"
#include "utils/utils.h"
#include <stdio.h>
#include <math.h>

bool sliderUpdate(Slider_St* slider, Vector2 mouseScreen) {
    if (slider->state == WIDGET_STATE_DISABLED) return false;

    bool hovered = CheckCollisionPointRec(mouseScreen, slider->bounds);

    if (hovered) {
        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
            slider->state = WIDGET_STATE_CLICK;
        } else {
            slider->state = WIDGET_STATE_HOVERED;
        }
    } else if (slider->state != WIDGET_STATE_CLICK) {
        slider->state = WIDGET_STATE_NORMAL;
    }

    if (slider->state == WIDGET_STATE_CLICK && IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
        f32 relativeX = mouseScreen.x - slider->bounds.x;
        f32 t = relativeX / slider->bounds.width;
        t = clamp(t, 0.0f, 1.0f);

        f32 oldValue = slider->value;
        slider->value = slider->minValue + t * (slider->maxValue - slider->minValue);

        return (fabsf(slider->value - oldValue) > 0.0001f);
    }

    if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
        slider->state = hovered ? WIDGET_STATE_HOVERED : WIDGET_STATE_NORMAL;
    }

    return false;
}

void sliderDraw(const Slider_St* slider, Font font, f32 fontSize) {
    DrawRectangleRec(slider->bounds, LIGHTGRAY);
    DrawRectangleLinesEx(slider->bounds, 2.0f, DARKGRAY);

    f32 range = slider->maxValue - slider->minValue;
    f32 t = (range > 0.0f) ? (slider->value - slider->minValue) / range : 0.0f;
    t = clamp(t, 0.0f, 1.0f);

    Rectangle filled = slider->bounds;
    filled.width = filled.width * t;

    Color fillColor = (slider->state == WIDGET_STATE_CLICK) ? SKYBLUE
                    : (slider->state == WIDGET_STATE_HOVERED) ? Fade(SKYBLUE, 0.9f)
                    : BLUE;

    DrawRectangleRec(filled, fillColor);

    f32 knobRadius = slider->bounds.height * 0.6f;
    Vector2 knobCenter = {
        slider->bounds.x + filled.width,
        slider->bounds.y + slider->bounds.height * 0.5f
    };

    DrawCircleV(knobCenter, knobRadius, WHITE);
    DrawCircleLinesV(knobCenter, knobRadius, (slider->state == WIDGET_STATE_CLICK) ? YELLOW : DARKGRAY);

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
