/**
    @file editor/utils.h
    @author Grok (assisted) + Fshimi-Hawlk
    @date 2026-03-27
    @date 2026-03-27
    @brief Pure logic utilities used only by the level editor module.

    All functions here are side-effect free where possible.
*/

#ifndef EDITOR_UTILS_H
#define EDITOR_UTILS_H

#include "utils/userTypes.h"

/**
    @brief Returns true if the world point is inside the terrain rectangle.

    Simple point-vs-rectangle test using Raylib's `CheckCollisionPointRec`.
*/
bool pointInTerrain(const LobbyTerrain_St* const terrain, Vector2 point);

/**
    @brief Returns the index of the first terrain that contains the point, or -1 if none.

    Searches through the global `terrains` dynamic array in order.
    Returns the lowest index that contains the point (useful for selection priority).
*/
s32 findTerrainAtPoint(Vector2 point);

Color getTerrainTypeColor(TerrainType_Et type);

/**
    @brief Creates a new terrain with default size and per-type defaults.

    @param type      Terrain type that determines color and roundness
    @param position  Top-left corner of the new terrain
    @return          Fully initialized `LobbyTerrain_St` with sensible defaults
*/
LobbyTerrain_St createDefaultTerrain(TerrainType_Et type, Vector2 position);

/**
    @brief Returns true if two terrain rectangles overlap.

    Uses Raylib's `CheckCollisionRecs` on the `rect` fields.
*/
bool terrainsOverlap(const LobbyTerrain_St* const a, const LobbyTerrain_St* const b);

/* ── Editor-specific helpers ── */

/**
    @brief Computes mouse position in world space using the current camera.

    Convenience wrapper around `GetScreenToWorld2D(GetMousePosition(), game->cam)`.
*/
Vector2 getMouseWorld(const LobbyGame_St* const game);

/**
    @brief Fills an array of 8 resize handle rectangles for a given terrain rect.

    Handles are ordered as:
    0 = top-left, 1 = top, 2 = top-right,
    3 = right, 4 = bottom-right, 5 = bottom,
    6 = bottom-left, 7 = left.

    @param handles    Output array of 8 Rectangles (must be valid)
    @param r          Source terrain rectangle
    @param handleSize Size of each handle in world units (usually scaled by 1/zoom)
*/
void fillResizeHandles(Rectangle handles[8], Rectangle r, f32 handleSize);

/**
    @brief Computes the tight axis-aligned bounding box (AABB) of all currently selected terrains.

    Uses the classic AABB union algorithm:
    1. Start with the rectangle of the first selected terrain.
    2. For every subsequent selected terrain, expand the current box:
       - new_min_x = min(current_min_x, terrain_min_x)
       - new_min_y = min(current_min_y, terrain_min_y)
       - new_max_x = max(current_max_x, terrain_max_x)
       - new_max_y = max(current_max_y, terrain_max_y)
    3. Convert back to position + size: width = max_x - min_x, height = max_y - min_y

    This produces the smallest rectangle that fully contains every selected terrain,
    independent of selection order (including Shift+click multi-select).

    @return Bounding box of the selection, or {0} if no terrains are selected
*/
Rectangle computeSelectedGroupBox(void);

/**
    @brief Computes the tight axis-aligned bounding box (AABB) of the clipboard contents.

    Uses the same AABB union algorithm as `computeSelectedGroupBox()` but operates on
    the `clipboard` dynamic array instead of selected indices.

    Used for paste positioning (to calculate correct anchor offsets).

    @return Bounding box of all items in the clipboard, or {0} if clipboard is empty
*/
Rectangle computeClipboardGroupBox(void);

/**
    @brief Returns the anchor offset based on pasteAnchorIndex (0-8, 3x3 grid).

    The 3x3 grid is indexed as follows:
    ```
    0 1 2
    3 4 5
    6 7 8
    ```
    Index 4 is the center (default).

    @param groupBox     Bounding box of the group being pasted (used to compute half-width/height)
    @param anchorIndex  0-8 index into the 3x3 anchor grid
    @return             Offset from the paste position to the chosen anchor point
*/
Vector2 getPasteAnchorOffset(Rectangle groupBox, s32 anchorIndex);

/**
    @brief Moves all selected terrains by the given world offset.

    Iterates over `selectedIndices` and adds `offset` to each terrain's `rect.x` and `rect.y`.
    Used during live multi-select dragging.
*/
void moveSelectedByOffset(Vector2 offset);

/**
    @brief Returns a preview rectangle for the currently edited terrain using widget buffers.
           Falls back to the real terrain rect if no live edit is active.
    @param idx Index of the terrain being edited
    @return Preview rectangle (with live values from text boxes when editing)
*/
Rectangle getEditedTerrainPreviewRect(s32 idx);

f32 snapToGrid(f32 v);

/**
    @brief Draws a temporary colored border around a portal (used for visual feedback after selection).
    @param idx      Terrain index to highlight
    @param color    Color of the border
    @param cam      Current camera (to scale line thickness)
*/
void drawPortalHighlight(s32 idx, Color color, const Camera2D* cam);

#endif // EDITOR_UTILS_H