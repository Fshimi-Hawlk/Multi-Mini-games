/**
    @file background.h
    @author Léandre BAUDET
    @date 2026-04-14
    @date 2026-04-14
    @brief background.h implementation/header file
*/
#ifndef UI_BACKGROUND_H
#define UI_BACKGROUND_H

#include "raylib.h"

/**
    @brief Description for lobby_drawSceneBackground
    @param[in,out] time The time parameter
    @param[in,out] playerPos The playerPos parameter
*/
void lobby_drawSceneBackground(float time, Vector2 playerPos);

/**
    @brief Description for lobby_initBackgroundScale
*/
void lobby_initBackgroundScale(void);

/**
    @brief Draws the parallax starry background using starry-background.png.
    The texture is scaled once to a fixed world size.
    It is horizontally centered on the camera (with slow parallax),
    while its bottom edge is **exactly anchored** at `GROUND_Y + 1000`
    as you requested.
    No complex tiling (texture is not seamless) → single large DrawTexturePro.

    @param playerPos   Player world position (used for parallax offset)
    @param camera      Current camera (used to center horizontally)
*/
void lobby_drawStarryBackground(const Vector2 playerPos, const Camera2D camera);

#endif // UI_BACKGROUND_H