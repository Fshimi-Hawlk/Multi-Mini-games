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

#include "raylib.h"

#ifndef f32Vector2_def
#define f32Vector2_def      // marker to indicate the typedef is active
/**
    @brief Shorthand alias for Raylib's Vector2 (used in many places for clarity).
 */
typedef Vector2 f32Vector2;
#endif

#include "baseTypes.h"

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
    f32 scrollY;            ///< Current scroll offset (negative = content moved up)
    f32 contentHeight;      ///< Total virtual height of the content
    f32 visibleHeight;      ///< Height of the scissor area (updated each frame)
    f32 scrollSpeed;        ///< Pixels per wheel unit (default 60.0f)
    f32 roundness;
} ScrollFrame_St;

// ── TextBox ──────────────────────────────────────────────────────────────────

typedef struct {
    Rectangle      bounds;
    WidgetState_Et state;
    char           buffer[256];
    u32            cursorPos;
    bool           editMode;
    f32            roundness;
    const char    *placeholder;     ///< Shown when buffer empty and not active
    bool           isValid;         ///< Validation flag - controls active border color (green/red)
} TextBox_St;

// ── Button (text and image variants) ─────────────────────────────────────────

typedef struct {
    Rectangle      bounds;
    WidgetState_Et state;
    const char    *text;        ///< static or owned by caller
    Color          baseColor;
    f32            roundness;
} TextButton_St;

typedef struct {
    Rectangle      bounds;
    WidgetState_Et state;
    Texture        texture;
    Color          tint;
    f32            roundness;
} ImageButton_St;

// ── Slider ───────────────────────────────────────────────────────────────────

typedef struct {
    Rectangle      bounds;
    WidgetState_Et state;
    f32            value;       ///< 0.0 .. 1.0
    f32            minValue;
    f32            maxValue;
    f32            roundness;
} Slider_St;

// ── DropDown ────────────────────────────────────────────────

typedef struct {
    Rectangle      bounds;
    WidgetState_Et state;
    s32            selectedIndex;
    bool           isOpen;
    f32            roundness;
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
    f32            roundness;
} CheckBox_St;

#endif // WIDGETS_TYPES_H