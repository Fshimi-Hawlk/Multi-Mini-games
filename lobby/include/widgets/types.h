/**
    @file widgets/types.h
    @author Fshimi-Hawlk
    @date 2026-03-30
    @brief Core type definitions for the enhanced reusable lobby widget system.
*/

#ifndef WIDGETS_TYPES_H
#define WIDGETS_TYPES_H

#include "utils/userTypes.h"

/**
    @brief Common interactive states for UI elements.
*/
typedef enum {
    WIDGET_STATE_NORMAL,
    WIDGET_STATE_HOVERED,
    WIDGET_STATE_CLICK,
    WIDGET_STATE_DISABLED
} WidgetState_Et;

/**
    @brief Reusable text button widget.
*/
typedef struct {
    Rectangle      bounds;
    WidgetState_Et state;
    char           text[64];
    Color          baseColor;
    f32            roundness;
} TextButton_St;

/**
    @brief Reusable image button widget.
*/
typedef struct {
    Rectangle      bounds;
    WidgetState_Et state;
    Texture2D      texture;
    Color          baseColor;
    f32            roundness;
} ImageButton_St;

/**
    @brief Interactive single-line text input field.
*/
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

/**
    @brief Interactive check box widget.
*/
typedef struct {
    Rectangle      bounds;
    WidgetState_Et state;
    bool           checked;
    char           text[64];
} CheckBox_St;

/**
    @brief Interactive slider widget.
*/
typedef struct {
    Rectangle      bounds;
    WidgetState_Et state;
    f32            value;
    f32            minValue;
    f32            maxValue;
    char           text[64];
} Slider_St;

/**
    @brief Interactive dropdown menu widget.
*/
typedef struct {
    Rectangle      bounds;
    WidgetState_Et state;
    const char**   options;
    u32            count;
    s32            selectedIndex;
    bool           isOpen;
} Dropdown_St;

/**
    @brief Container for scrollable content.
*/
typedef struct {
    Rectangle      bounds;
    Rectangle      contentRect;
    Vector2        scroll;
} ScrollFrame_St;

#endif // WIDGETS_TYPES_H
