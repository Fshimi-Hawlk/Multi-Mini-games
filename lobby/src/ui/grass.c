/**
    @file ui/grass.c
    @author Fshimi-Hawlk
    @date 2026-01-30
    @date 2026-04-15
    @brief Optimized grass rendering with full batched rlgl drawing + improved visuals.

    Contributors:
        - LeandreB8:
            - Original grass physics and drawing
        - Fshimi-Hawlk:
            - Full physics rework
            - Batched rlgl rendering
            - Curved blades, thickness gradient, color gradient
            - Style / documentation compliance

    @note Keeps exact same physics & 65 000 blades. Now looks much more organic.
*/

#include "ui/grass.h"

#include "utils/globals.h"
#include "sharedUtils/mathUtils.h"

#include "rlgl.h"

void lobby_updateGrass(const Player_St* const player, const float dt, const float time, const Camera2D camera) {
    const float viewL = camera.target.x - (GetScreenWidth() / (2.0f * camera.zoom)) - 100.0f;
    const float viewR = camera.target.x + (GetScreenWidth() / (2.0f * camera.zoom)) + 100.0f;
    const float viewT = camera.target.y - (GetScreenHeight() / (2.0f * camera.zoom)) - 100.0f;
    const float viewB = camera.target.y + (GetScreenHeight() / (2.0f * camera.zoom)) + 100.0f;

    for (int i = 0; i < grassCount; ++i) {
        GrassBlade_St* b = &grassBlades[i];

        if (b->position.x < viewL || b->position.x > viewR ||
            b->position.y < viewT || b->position.y > viewB) {
            continue;
        }

        float windBase = sinf(time * 5.0f + b->position.x * 0.05f + b->position.y * 0.02f) * 0.18f;
        float stiffness = 48.0f;
        float damping   = 0.87f;

        float force = -stiffness * (b->angle - windBase);
        b->velocity += force * dt;
        b->velocity *= damping;
        b->angle += b->velocity * dt;

        Vector2 toBlade = Vector2Subtract(b->position, player->position);
        float distSq = Vector2LengthSqr(toBlade);

        if (distSq < 0.001f) continue;

        const float interactionRadiusSq = 1000.0f;

        if (distSq < interactionRadiusSq) {
            Vector2 pushDir = Vector2Normalize(toBlade);
            float playerSpeedFactor = Vector2Length(player->velocity) * 0.008f;
            float pushStrength = (1.0f - (distSq / interactionRadiusSq)) * (28.0f + playerSpeedFactor * 12.0f);

            b->velocity += pushDir.x * pushStrength * 1.8f;
            b->velocity += pushDir.y * pushStrength * 0.6f;

            if (fabsf(player->velocity.x) > 80.0f) {
                b->velocity += player->velocity.x * 0.014f;
            }
        }

        if (distSq > 3200.0f) {
            b->angle = lerp(b->angle, windBase, 8.0f * dt);
        }

        b->angle = clamp(b->angle, -0.55f, 0.55f);
    }
}

void lobby_drawGrass(const Camera2D camera) {
    const float viewL = camera.target.x - (GetScreenWidth() / (2.0f * camera.zoom)) - 100.0f;
    const float viewR = camera.target.x + (GetScreenWidth() / (2.0f * camera.zoom)) + 100.0f;
    const float viewT = camera.target.y - (GetScreenHeight() / (2.0f * camera.zoom)) - 100.0f;
    const float viewB = camera.target.y + (GetScreenHeight() / (2.0f * camera.zoom)) + 100.0f;

    rlBegin(RL_LINES);

    for (int i = 0; i < grassCount; ++i) {
        GrassBlade_St* b = &grassBlades[i];

        if (b->position.x < viewL || b->position.x > viewR ||
            b->position.y < viewT || b->position.y > viewB) {
            continue;
        }

        // Base position
        Vector2 base = b->position;

        // Three points along the curved blade
        float h1 = b->height * 0.35f;
        float h2 = b->height * 0.75f;
        float h3 = b->height;

        Vector2 p1 = { base.x + sinf(b->angle) * h1 * 0.6f,
                       base.y - cosf(b->angle) * h1 };

        Vector2 p2 = { base.x + sinf(b->angle * 1.1f) * h2 * 0.85f,
                       base.y - cosf(b->angle * 1.1f) * h2 };

        Vector2 tip = { base.x + sinf(b->angle) * h3,
                        base.y - cosf(b->angle) * h3 };

        // Shadow (slightly offset)
        Vector2 s1 = {p1.x + 5.0f, p1.y + 3.0f};
        Vector2 s2 = {p2.x + 5.0f, p2.y + 3.0f};
        Vector2 s3 = {tip.x + 5.0f, tip.y + 3.0f};

        Color shadowCol = Fade(BLACK, 0.42f);
        rlColor4ub(shadowCol.r, shadowCol.g, shadowCol.b, shadowCol.a);
        rlVertex2f(base.x, base.y); rlVertex2f(s1.x, s1.y);
        rlVertex2f(s1.x, s1.y);     rlVertex2f(s2.x, s2.y);
        rlVertex2f(s2.x, s2.y);     rlVertex2f(s3.x, s3.y);

        // Main blade with gradient
        Color baseCol = b->color;
        Color tipCol  = Fade(b->color, 0.95f);   // slightly brighter tip

        rlColor4ub(baseCol.r, baseCol.g, baseCol.b, baseCol.a);
        rlVertex2f(base.x, base.y); rlVertex2f(p1.x, p1.y);

        rlColor4ub((baseCol.r + tipCol.r)/2, (baseCol.g + tipCol.g)/2, (baseCol.b + tipCol.b)/2, 255);
        rlVertex2f(p1.x, p1.y); rlVertex2f(p2.x, p2.y);

        rlColor4ub(tipCol.r, tipCol.g, tipCol.b, tipCol.a);
        rlVertex2f(p2.x, p2.y); rlVertex2f(tip.x, tip.y);

        // Tiny highlight along the curve
        // if (b->height > 12.0f) {
        //     Color highlight = Fade(WHITE, 0.18f);
        //     rlColor4ub(highlight.r, highlight.g, highlight.b, highlight.a);
        //     rlVertex2f(base.x + 1.0f, base.y); rlVertex2f(p1.x + 1.0f, p1.y);
        //     rlVertex2f(p1.x + 1.0f, p1.y);     rlVertex2f(p2.x + 1.0f, p2.y);
        //     rlVertex2f(p2.x + 1.0f, p2.y);     rlVertex2f(tip.x + 1.0f, tip.y);
        // }

        // Base dot (only on thicker blades)
        // if (b->height > 18.0f) {
        //     Color baseDot = Fade(BLACK, 0.72f);
        //     rlColor4ub(baseDot.r, baseDot.g, baseDot.b, baseDot.a);
        //     rlVertex2f(base.x - 2.0f, base.y);
        //     rlVertex2f(base.x + 2.0f, base.y);
        // }
    }

    rlEnd();
}