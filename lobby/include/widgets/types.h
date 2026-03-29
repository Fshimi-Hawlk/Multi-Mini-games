/**
    @file widgets/types.h
    @author Fshimi-Hawlk
    @date 2026-03-28
    @brief Core types and enums for the reusable widget system.

    All widget-related public types live here following project typedef rules:
      - No suffix for primitive-feeling types
      - _St for structs
      - _Et for enums
*/

#ifndef WIDGETS_TYPES_H
#define WIDGETS_TYPES_H

#include "utils/userTypes.h"   // for Rectangle, Vector2, Color, etc.

// ── Common widget states ─────────────────────────────────────────────────────

/**
    @brief Shared state for all interactive widgets.
*/
typedef enum {
    WIDGET_STATE_NORMAL,
    WIDGET_STATE_HOVER,
    WIDGET_STATE_ACTIVE,      ///< Focused / editing
    WIDGET_STATE_CLICK,
    WIDGET_STATE_DISABLED
} WidgetState_Et;

// ── ScrollFrame ──────────────────────────────────────────────────────────────

/**
    @brief Reusable scrollable container.
           Owns its scroll offset and handles wheel input + clamping.
*/
typedef struct {
    f32 scrollY;           ///< Current scroll offset (negative = content moved up)
    f32 contentHeight;     ///< Total virtual height of the content
    f32 visibleHeight;     ///< Height of the scissor area (updated each frame)
    f32 scrollSpeed;       ///< Pixels per wheel unit (default 60.0f)
} ScrollFrame_St;

// ── TextBox ──────────────────────────────────────────────────────────────────

typedef struct {
    Rectangle      bounds;
    WidgetState_Et state;
    char           buffer[256];
    u32            cursorPos;
    bool           editMode;
} TextBox_St;

// ── Button (text and image variants) ─────────────────────────────────────────

typedef struct {
    Rectangle      bounds;
    WidgetState_Et state;
    const char*    text;        ///< static or owned by caller
    Color          baseColor;
} TextButton_St;

typedef struct {
    Rectangle      bounds;
    WidgetState_Et state;
    Texture        texture;
    Color          tint;
} ImageButton_St;

// ── Slider ───────────────────────────────────────────────────────────────────

typedef struct {
    Rectangle      bounds;
    WidgetState_Et state;
    f32            value;       ///< 0.0 .. 1.0
    f32            minValue;
    f32            maxValue;
} Slider_St;

// ── DropDown ────────────────────────────────────────────────

typedef struct {
    Rectangle      bounds;
    WidgetState_Et state;
    s32            selectedIndex;
    bool           isOpen;
} DropDown_St;

// ── CheckBox ────────────────────────────────────────────────

/**
    @brief Simple checkbox widget (toggle on/off).
*/
typedef struct {
    Rectangle      bounds;
    WidgetState_Et state;
    bool           checked;
    const char*    label;        ///< Optional label text to the right of the box
} CheckBox_St;

#endif // WIDGETS_TYPES_H