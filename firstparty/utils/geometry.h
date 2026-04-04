/**
    @file firstparty/utils/geometry.h
    @author Fshimi-Hawlk
    @date 2026-04-03
    @date 2026-04-03
    @brief Rectangle anchoring, creation helpers and high-level drawing utilities.

    Provides consistent ways to position rectangles and text using anchor points
    (useful for UI, HUD, menus, tooltips, etc.).

    @see geometry.c for implementation of drawing functions
*/

#ifndef FIRSTPARTY_UTILS_GEOMETRY_H
#define FIRSTPARTY_UTILS_GEOMETRY_H

#include "raylib.h"

/**
    @brief Shorthand alias for Raylib's Vector2 (used in many places for clarity).
 */
typedef Vector2 f32Vector2;
#define f32Vector2_def      // marker to indicate the typedef is active

/**
    @brief Nine-point anchor positions for rectangles and text.
*/
typedef enum {
    ANCHOR_TOP_LEFT,    ANCHOR_TOP,     ANCHOR_TOP_RIGHT,
    ANCHOR_LEFT,        ANCHOR_CENTER,  ANCHOR_RIGHT,
    ANCHOR_BOTTOM_LEFT, ANCHOR_BOTTOM,  ANCHOR_BOTTOM_RIGHT
} Anchor_Et;

/**
    @brief Convenience macro to create a Rectangle from a position Vector2 and a size Vector2.

    Equivalent to: (Rectangle){ .x = pos.x, .y = pos.y, .width = size.x, .height = size.y }
*/
#define createRect(pos, size) (Rectangle) { .x = (pos).x, .y = (pos).y, .width = (size).x, .height = (size).y }

#define getRectPos(rec) (Vector2) {.x = (rec).x, .y = (rec).y}
#define getRectSize(rec) (Vector2) {.x = (rec).width, .y = (rec).height}

#define scaleRec(rec, scalor) (Rectangle) { \
    .x = (rec).x,                           \
    .y = (rec).y,                           \
    .width = (rec).width * scalor,          \
    .height = (rec).height * scalor,        \
}

/**
    @brief Returns the full source rectangle of a texture ({0, 0, w, h}).

    Used consistently as the source rect in DrawTexture* calls.
    Acts as a future-proof hook for sprite-sheet or sub-region support.

    @param texture Valid Raylib Texture2D
    @return Rectangle spanning the entire texture
*/
Rectangle getTextureRec(const Texture texture);

/**
    @brief Draws text with the given anchor point.

    The `pos` parameter is the anchor point, not necessarily the top-left corner.
    Uses `font.baseSize` as font size and 0 spacing (standard for most UI text).

    @param[in] text   Null-terminated string to draw
    @param[in] font   Loaded Raylib font
    @param[in] pos    Anchor position in world/screen space
    @param[in] anchor Which point of the text bounding box should be placed at `pos`
    @param[in] color  Text color
*/
void drawTextPro(const char* text, Font font, f32Vector2 pos, Anchor_Et anchor, Color color);

#endif // FIRSTPARTY_UTILS_GEOMETRY_H