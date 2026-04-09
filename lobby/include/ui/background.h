#ifndef BACKGROUND_H
#define BACKGROUND_H

#include "raylib.h"

void drawSceneBackground(float time, Vector2 playerPos);

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
void drawStarryBackground(const Vector2 playerPos, const Camera2D camera);

#endif // BACKGROUND_H