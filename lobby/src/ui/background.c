#include "ui/background.h"
#include "utils/configs.h"
#include "utils/globals.h"

void drawSceneBackground(float time, Vector2 playerPos) {
    (void) time;

    float parallaxFactor = 0.15f;

    float pX = playerPos.x * parallaxFactor;
    float pY = playerPos.y * parallaxFactor;

    Color skyTop = (Color){ 25, 84, 157, 255 };
    Color skyMid = (Color){ 125, 195, 230, 255 };
    Color skyLow = (Color){ 210, 240, 255, 255 };

    float skyLeft = (playerPos.x - SKY_WIDTH / 2.0f) + pX;

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

void drawStarryBackground(const Vector2 playerPos, const Camera2D camera) {
    if (!IsTextureValid(texBackground)) {
        // fallback to old gradient sky
        drawSceneBackground(0.0f, playerPos);
        return;
    }

    // ── Configuration ───────────────────────────────────────────────────────
    const Vector2 parallax = {-0.035f, -0.05f};
    const float scale    = 0.35f;

    float bgWidth  = (float)texBackground.width  * scale;
    float bgHeight = (float)texBackground.height * scale;

    // ── Parallax offset ────────
    Vector2 offset = Vector2Multiply(playerPos, parallax);

    // ── Center horizontally on camera ───────────────────────────────────────
    Vector2 center = Vector2Add(camera.target, offset);

    // Final destination rectangle (centered horizontally, bottom-anchored vertically)
    Rectangle dest = {
        .x      = center.x - bgWidth / 2.0f,
        .y      = center.y - bgHeight / 2.0f - 250,
        .width  = bgWidth,
        .height = bgHeight
    };

    // ── Draw the single scaled background ───────────────────────────────────
    DrawTexturePro(
        texBackground,
        (Rectangle){0.0f, 0.0f, (float)texBackground.width, (float)texBackground.height},
        dest,
        Vector2Zero(),
        0.0f,
        WHITE
    );
}
