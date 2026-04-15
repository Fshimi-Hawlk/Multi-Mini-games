/**
    @file background.c
    @author Léandre BAUDET
    @date 2026-04-14
    @date 2026-04-14
    @brief background.c implementation/header file
*/
#include "ui/background.h"

#include "utils/globals.h"

void lobby_drawSceneBackground(f32 time, Vector2 playerPos) {
    (void) time;

    f32 parallaxFactor = 0.15f;

    f32 pX = playerPos.x * parallaxFactor;
    f32 pY = playerPos.y * parallaxFactor;

    Color skyTop = (Color){ 25, 84, 157, 255 };
    Color skyMid = (Color){ 125, 195, 230, 255 };
    Color skyLow = (Color){ 210, 240, 255, 255 };

    f32 skyLeft = (playerPos.x - SKY_WIDTH / 2.0f) + pX;

    DrawRectangleGradientV(
        skyLeft,
        -SKY_HEIGHT + pY,
        SKY_WIDTH,
        SKY_HEIGHT,
        skyTop,
        skyMid
    );

    DrawRectangleGradientV(
        skyLeft,
        0 + pY,
        SKY_WIDTH,
        GROUND_Y + 1000,
        skyMid,
        skyLow
    );
}

static f32 backgroundBaseScale = 0.35f;   // computed once

void lobby_initBackgroundScale(void) {
    if (!IsTextureValid(backgroundTexture)) return;

    f32 windowAspect = (f32) systemSettings.video.width / (f32) systemSettings.video.height;
    f32 bgAspect     = (f32) backgroundTexture.width / (f32) backgroundTexture.height;

    const f32 targetCoverage = 1.75f;

    if (windowAspect > bgAspect) { // wider window -> fit to height
        backgroundBaseScale = (f32) systemSettings.video.height / (f32) backgroundTexture.height * targetCoverage;
    } else { // taller window -> fit to width
        backgroundBaseScale = (f32) systemSettings.video.width / (f32) backgroundTexture.width * targetCoverage;
    }
}

void lobby_drawStarryBackground(const Vector2 playerPos, const Camera2D camera) {
    if (!IsTextureValid(backgroundTexture)) {
        // fallback to old gradient sky
        lobby_drawSceneBackground(0.0f, playerPos);
        return;
    }

    f32 bgWidth  = (f32)backgroundTexture.width  * backgroundBaseScale;
    f32 bgHeight = (f32)backgroundTexture.height * backgroundBaseScale;

    // ── Parallax ───────────────────────────────────────────────────────────
    const Vector2 parallax = {-0.035f, -0.05f};
    Vector2 offset = Vector2Multiply(playerPos, parallax);

    Vector2 center = Vector2Add(camera.target, offset);

    // Final destination (centered horizontally, vertical anchor)
    Rectangle dest = {
        .x      = center.x - bgWidth / 2.0f,
        .y      = center.y - bgHeight / 2.0f,
        .width  = bgWidth,
        .height = bgHeight
    };

    DrawTexturePro(
        backgroundTexture,
        (Rectangle){0.0f, 0.0f, (f32)backgroundTexture.width, (f32)backgroundTexture.height},
        dest,
        Vector2Zero(),
        0.0f,
        WHITE
    );
}
