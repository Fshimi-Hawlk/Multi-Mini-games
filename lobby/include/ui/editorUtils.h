/**
    @file ui/editorUtils.h
    @author Fshimi-Hawlk
    @date 2026-03-27
    @brief Pure logic utilities used only by the level editor.

    All functions are independent of Raylib input/drawing and global state where possible.
*/

#ifndef UI_EDITOR_UTILS_H
#define UI_EDITOR_UTILS_H

#include "utils/userTypes.h"

/**
    @brief Returns true if the world point is inside the terrain rectangle (including border).
*/
bool pointInTerrain(const LobbyTerrain_St* const terrain, Vector2 point);

/**
    @brief Returns the index of the first terrain that contains the point, or -1 if none.
*/
s32 findTerrainAtPoint(Vector2 point);

/**
    @brief Creates a new terrain with default size (200x30) and reasonable defaults.
*/
LobbyTerrain_St createDefaultTerrain(TerrainType_Et type, Vector2 position);

/**
    @brief Returns true if two terrain rectangles overlap.
*/
bool terrainsOverlap(const LobbyTerrain_St* const a, const LobbyTerrain_St* const b);

#endif // UI_EDITOR_UTILS_H