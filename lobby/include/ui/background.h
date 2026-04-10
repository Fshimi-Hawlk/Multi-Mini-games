#ifndef UI_BACKGROUND_H
#define UI_BACKGROUND_H

#include "raylib.h"

void lobby_drawSceneBackground(float time, Vector2 playerPos);

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