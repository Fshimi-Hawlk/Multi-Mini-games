/**
    @file geometry.h
    @author Multi Mini-Games Team
    @date 2026-04-03
    @date 2026-04-14
    @brief Rectangle anchoring, creation helpers and high-level drawing utilities.
*/
#ifndef FIRSTPARTY_UTILS_GEOMETRY_H
#define FIRSTPARTY_UTILS_GEOMETRY_H

#include "raylib.h"

#ifndef f32Vector2_def
#define f32Vector2_def      // marker to indicate the typedef is active
/**
    @brief Shorthand alias for Raylib's Vector2 (used in many places for clarity).
 */
typedef Vector2 f32Vector2;
#endif

/**
    @brief Nine-point anchor positions for rectangles and text.
*/
typedef enum {
    ANCHOR_TOP_LEFT,    ANCHOR_TOP,     ANCHOR_TOP_RIGHT,
    ANCHOR_LEFT,        ANCHOR_CENTER,  ANCHOR_RIGHT,
    ANCHOR_BOTTOM_LEFT, ANCHOR_BOTTOM,  ANCHOR_BOTTOM_RIGHT
} Anchor_Et;

#define RECT_STR "(%f, %f) => {%f, %f}"
#define RECT_FMT(rect) rect.x, rect.y, rect.width, rect.height

/**
    @brief Convenience macro to create a Rectangle from a position Vector2 and a size Vector2.

    Equivalent to: (Rectangle){ .x = pos.x, .y = pos.y, .width = size.x, .height = size.y }
*/
#define createRect(pos, size) (Rectangle) { .x = (pos).x, .y = (pos).y, .width = (size).x, .height = (size).y }

#define getRectPos(rect) (Vector2) {.x = (rect).x, .y = (rect).y}
#define getRectSize(rect) (Vector2) {.x = (rect).width, .y = (rect).height}
#define getRectCenterPos(rect) (Vector2) {(rect).x + (rect).width / 2.0f, (rect).y + (rect).height / 2.0f}

/**
    @brief Returns the full source rectangle of a texture ({0, 0, w, h}).

    Used consistently as the source rect in DrawTexture* calls.
    Acts as a future-proof hook for sprite-sheet or sub-region support.

    @param texture Valid Raylib Texture2D
    @return Rectangle spanning the entire texture
*/
#define getTextureRec(texture) (Rectangle) { 0, 0, (f32) (texture).width, (f32) (texture).height }

#define scaleRect(rect, scalor) (Rectangle) { \
    .x = (rect).x,                           \
    .y = (rect).y,                           \
    .width = (rect).width * scalor,          \
    .height = (rect).height * scalor,        \
}

/**
    @brief Description for getAnchoredRect
    @param[in,out] rect The rect parameter
    @param[in,out] anchor The anchor parameter
    @return Success/failure or the result of the function
*/
Rectangle getAnchoredRect(Rectangle rect, Anchor_Et anchor);

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