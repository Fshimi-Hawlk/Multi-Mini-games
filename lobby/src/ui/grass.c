#include "ui/grass.h"

#include "utils/globals.h"
#include "utils/utils.h"

void lobby_updateGrass(const Player_St* const player, const float dt, const float time, const Camera2D camera) {
    const float viewL = camera.target.x - (WINDOW_WIDTH / 2.0f) - 100.0f;
    const float viewR = camera.target.x + (WINDOW_WIDTH / 2.0f) + 100.0f;
    const float viewT = camera.target.y - (WINDOW_HEIGHT / 2.0f) - 100.0f;
    const float viewB = camera.target.y + (WINDOW_HEIGHT / 2.0f) + 100.0f;

    for (int i = 0; i < grassCount; ++i) {
        GrassBlade_St* b = &grassBlades[i];

        if (b->position.x < viewL || b->position.x > viewR ||
            b->position.y < viewT || b->position.y > viewB) {
            continue;
        }

        // ── Wind + spring physics ───────────────────────────────────────
        float windBase = sinf(time * 5.0f + b->position.x * 0.05f + b->position.y * 0.02f) * 0.18f;
        float stiffness = 48.0f;
        float damping   = 0.87f;

        float force = -stiffness * (b->angle - windBase);
        b->velocity += force * dt;
        b->velocity *= damping;
        b->angle += b->velocity * dt;

        // ── Player interaction ───────────────────────
        Vector2 toBlade = Vector2Subtract(b->position, player->position);
        float distSq = Vector2LengthSqr(toBlade);

        if (distSq < 0.001f) continue;

        const float interactionRadiusSq = 2200.0f;   // ~47 px radius

        if (distSq < interactionRadiusSq) {
            Vector2 pushDir = Vector2Normalize(toBlade);

            // Stronger push when player is moving fast
            float playerSpeedFactor = Vector2Length(player->velocity) * 0.008f;
            float pushStrength = (1.0f - (distSq / interactionRadiusSq)) * (28.0f + playerSpeedFactor * 12.0f);

            // Push the blade away from the player (both horizontally and vertically)
            b->velocity += pushDir.x * pushStrength * 1.8f;   // horizontal bias because grass bends sideways
            b->velocity += pushDir.y * pushStrength * 0.6f;   // small vertical component

            // Extra kick when player is moving right through it
            if (fabsf(player->velocity.x) > 80.0f) {
                b->velocity += player->velocity.x * 0.014f;
            }
        }

        // Gentle recovery toward wind when far from player
        if (distSq > 3200.0f) {
            b->angle = lerp(b->angle, windBase, 8.0f * dt);
        }

        b->angle = clamp(b->angle, -0.55f, 0.55f);
    }
}

void lobby_drawGrass(const Player_St* const player, const Camera2D camera) {
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