#include "ui/grass.h"

#include "utils/globals.h"
#include "utils/utils.h"

void updateGrass(const Player_St* const player, const float dt, const float time, const Camera2D camera) {
    float viewL = camera.target.x - (WINDOW_WIDTH / 2.0f) - 100.0f;
    float viewR = camera.target.x + (WINDOW_WIDTH / 2.0f) + 100.0f;
    float viewT = camera.target.y - (WINDOW_HEIGHT / 2.0f) - 100.0f;
    float viewB = camera.target.y + (WINDOW_HEIGHT / 2.0f) + 100.0f;

    for (int i = 0; i < grassCount; ++i) {
        GrassBlade_St* b = &grassBlades[i];
        if (b->position.x < viewL || b->position.x > viewR ||
            b->position.y < viewT || b->position.y > viewB) continue;

        // Wind + spring physics
        float windBase = sinf(time * 5.0f + b->position.x * 0.05f + b->position.y * 0.02f) * 0.15f;
        float stiffness = 50.0f;
        float damping = 0.85f;
        float force = -stiffness * (b->angle - windBase);
        b->velocity += force * dt;
        b->velocity *= damping;
        b->angle += b->velocity * dt;

        // Player interaction
        float dx = player->position.x - b->position.x;
        float dy = player->position.y - b->position.y;
        float distSq = dx*dx + dy*dy;
        if (distSq > 360000.0f) b->angle = sinf(time * 2.0f + b->position.x * 0.05f) * 0.1f;
        if (distSq < 2000.0f) b->velocity -= (dx * 1.5f);
        b->angle = clamp(b->angle, -1.0f, 1.0f);
    }
}

void drawGrass(const Player_St* const player, const Camera2D camera) {
    float skyLeft = player->position.x - SKY_WIDTH / 2.0f;
    DrawRectangleGradientV(skyLeft, GROUND_Y, SKY_WIDTH, 1000.0f,
                           (Color){45, 35, 25, 255}, (Color){20, 15, 10, 255});

    float viewL = camera.target.x - (WINDOW_WIDTH / 2.0f) - 100.0f;
    float viewR = camera.target.x + (WINDOW_WIDTH / 2.0f) + 100.0f;
    float viewT = camera.target.y - (WINDOW_HEIGHT / 2.0f) - 100.0f;
    float viewB = camera.target.y + (WINDOW_HEIGHT / 2.0f) + 100.0f;

    for (int i = 0; i < grassCount; ++i) {
        GrassBlade_St* b = &grassBlades[i];
        if (b->position.x < viewL || b->position.x > viewR ||
            b->position.y < viewT || b->position.y > viewB) continue;

        Vector2 tip = {
            b->position.x + sinf(b->angle) * b->height,
            b->position.y - cosf(b->angle) * b->height
        };

        float thickness = (b->height > 12.0f) ? 3.0f : 2.0f;

        // ── Shadow layer (darker, slightly offset) ───────────────────────
        Vector2 shadowTip = {tip.x + 6.0f, tip.y + 4.0f};
        DrawLineEx(b->position, shadowTip, thickness + 1.0f, Fade(BLACK, 0.45f));

        // ── Main lit blade ────────────────────────────────────────────────
        DrawLineEx(b->position, tip, thickness, b->color);

        // Tiny highlight on the light-facing side of the blade
        if (b->height > 10.0f) {
            DrawLineEx(b->position, tip, thickness * 0.4f, Fade(WHITE, 0.22f));
        }

        if (b->height > 20.0f) {
            DrawCircleV(b->position, 3.0f, (Color){0, 0, 0, 60});
        }
    }
}