#include "config.h"
#include "constant.h"
#include "global.h"

#include "grass.h"

void initGrass(void) {
    Rectangle floor = platforms[0].rect;
    grassCount = 0;

    float stepX = 3.0f;
    float stepY = 5.0f;

    for (float y = floor.y; y < floor.y + 200.0f; y += stepY) {
        for (float x = -X_LIMIT - 500; x < X_LIMIT + 500; x += stepX) {
            if (grassCount >= MAX_GRASS_BLADES) break;

            float offX = (float)(rand() % 15) - 7.5f;
            float offY = (float)(rand() % 10);

            float baseHeight;
            int type = rand() % 10;
            if (type < 7) baseHeight = 5.0f + (rand() % 10);
            else if (type < 9) baseHeight = 15.0f + (rand() % 15);
            else baseHeight = 2.0f + (rand() % 4);

            float depth = (y - floor.y) / floor.height;
            float colorVar = (float)(rand() % 35);

            grassBlades[grassCount] = (GrassBlade_st){
                .position = { x + offX, y + offY },
                .height = baseHeight,
                .angle = 0.0f,
                .velocity = 0.0f,
                .color = (Color){
                    (unsigned char)Clamp(35 + colorVar - (depth * 15), 10, 255),
                    (unsigned char)Clamp(90 + colorVar - (depth * 70), 20, 180),
                    (unsigned char)Clamp(25 - (depth * 10), 5, 255),
                    255
                }
            };
            grassCount++;
        }
    }
}

void updateGrass(Player_st* player, float dt, float time, Camera2D camera) {
    float viewL = camera.target.x - (WINDOW_WIDTH / 2) - 100.0f;
    float viewR = camera.target.x + (WINDOW_WIDTH / 2) + 100.0f;
    float viewT = camera.target.y - (WINDOW_HEIGHT / 2) - 100.0f;
    float viewB = camera.target.y + (WINDOW_HEIGHT / 2) + 100.0f;

    for (int i = 0; i < grassCount; ++i) {
        GrassBlade_st* b = &grassBlades[i];
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
        b->angle = Clamp(b->angle, -1.0f, 1.0f);
    }
}

void drawGrass(Player_st* player, Camera2D camera) {
    float skyLeft = player->position.x - SKY_WIDTH / 2.0f;
    DrawRectangleGradientV(skyLeft, GROUND_Y, SKY_WIDTH, 1000.0f,
                           (Color){45, 35, 25, 255}, (Color){20, 15, 10, 255});

    float viewL = camera.target.x - (WINDOW_WIDTH / 2) - 100.0f;
    float viewR = camera.target.x + (WINDOW_WIDTH / 2) + 100.0f;
    float viewT = camera.target.y - (WINDOW_HEIGHT / 2) - 100.0f;
    float viewB = camera.target.y + (WINDOW_HEIGHT / 2) + 100.0f;

    for (int i = 0; i < grassCount; ++i) {
        GrassBlade_st* b = &grassBlades[i];
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