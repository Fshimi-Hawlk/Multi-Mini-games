#include "setups/game.h"

#include "utils/globals.h"

static void initGrass(void) {
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

            grassBlades[grassCount] = (GrassBlade_St){
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

void lobby_initGame(void) {
    initGrass();

    game.playerVisuals.isTextureMenuOpen = false;
    game.playerVisuals.defaultTextureRect = (Rectangle) { 20, 60, 50, 50};

    game.player = (Player_St) {
        .position = {0, 300},
        .radius = 20,

        .angle = 0,

        .velocity = {0, 0},

        .onGround = false,
        .nbJumps = 0,

        .coyoteTime = 0.1f,
        .coyoteTimer = 0.1f,

        .jumpBuffer = 0.0f
    };

    game.cam = (Camera2D) {
        .target = game.player.position,
        .offset = (Vector2){WINDOW_WIDTH / 2.0f, WINDOW_HEIGHT * 0.66f},
        .zoom = 1.0f,
    };
}