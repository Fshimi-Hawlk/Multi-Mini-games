/**
    @file widgets/types.h
    @author Fshimi-Hawlk
    @date 2026-03-30
    @brief Core type definitions for the enhanced reusable lobby widget system.
*/

#ifndef WIDGETS_TYPES_H
#define WIDGETS_TYPES_H

#include "utils/userTypes.h"

typedef enum {
    WIDGET_STATE_NORMAL,
    WIDGET_STATE_HOVERED,
    WIDGET_STATE_CLICK,
    WIDGET_STATE_DISABLED
} WidgetState_Et;

typedef struct {
    Rectangle      bounds;
    WidgetState_Et state;
    char           text[64];
    Color          baseColor;
    f32            roundness;
} TextButton_St;

typedef struct {
    Rectangle      bounds;
    WidgetState_Et state;
    Texture2D      texture;
    Color          baseColor;
    f32            roundness;
} ImageButton_St;

typedef struct {
    Rectangle      bounds;
    WidgetState_Et state;
    char           buffer[256];
    u32            cursorPos;
    bool           editMode;
    f32            roundness;
    const char*    placeholder;
    bool           isValid;
} TextBox_St;

#endif // WIDGETS_TYPES_H
