#include "config.h"
#include "constant.h"
#include "global.h"

#include "audio.h"
#include "textures.h"

#include "player.h"
#include "skin.h"

#include "grass.h"
#include "background.h"
#include "ambiance.h"

void drawTree(void);
void drawPlatforms(Platform_st* platforms, int count);
void drawWorldBoundaries(Player_st* player);

Camera2D cam = {0};

int main(void) {
    srand(time(NULL));

    SetTraceLogLevel(LOG_WARNING);

    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Lobby Multi-Mini-Games");

    lobby_initAudio();
    lobby_initTextures();
    initGrass();
    initAtmosphericEffects();

    logoSkinButton = LoadTexture("assets/logoSkin.png");

    playerTextures[0] = LoadTexture("assets/earth.png"); playerTextureCount++;
    playerTextures[1] = LoadTexture("assets/trollFace.png"); playerTextureCount++;

    Player_st player = {
        .position = {0, 300},
        .radius = 20,

        .texture = NULL,
        .angle = 0,

        .velocity = {0, 0},

        .onGround = false,
        .nbJumps = 0,

        .coyoteTime = 0.1f,
        .coyoteTimer = 0.1f,

        .jumpBuffer = 0.0f
    };

    cam.target = player.position;
    cam.offset = (Vector2){WINDOW_WIDTH / 2, WINDOW_HEIGHT * 0.66};
    cam.zoom = 1.0f;

    while (!WindowShouldClose()) {
        float dt = GetFrameTime();
        gameTime += dt;

        if (gameTime > 1.45f)
            updatePlayer(&player, platforms, platformCount, dt);

        cam.target = player.position;

        toggleSkinMenu();

        if (isTextureMenuOpen) {
            choosePlayerTexture(&player);
        }

        updateGrass(&player, GetFrameTime(), gameTime, cam);
        updateAtmosphericEffects(dt, &player, cam);

        BeginDrawing(); {
            ClearBackground(RAYWHITE);

            BeginMode2D(cam); {
                // DrawSceneBackground(gameTime, player.position);
                drawStarryBackground(player.position, cam);

                drawTree();
                drawPlatforms(platforms, platformCount);
                drawWorldBoundaries(&player);

                drawGrass(&player, cam);

                drawAtmosphericEffects();

                drawPlayer(&player);
                // DrawPlatforms(platforms, platformCount);
            } EndMode2D();
            DrawText("Multi-Mini-Games", WINDOW_WIDTH / 2 - MeasureText("Multi-Mini-Games", 20) / 2.0f, 20, 20, PURPLE);
            drawSkinButton();
            if (isTextureMenuOpen) {
                drawMenuTextures();
            }

        } EndDrawing();
    }

    for (int i = 0; i < playerTextureCount; i++)
        UnloadTexture(playerTextures[i]);

    UnloadTexture(logoSkinButton);

    lobby_freeAudio();
    lobby_freeTextures();

    CloseWindow();
    return 0;
}

void drawTree(void) {
    if (!IsTextureValid(texTree)) return;

    float treeScale = 0.7f;
    float drawWidth  = (float)texTree.width  * treeScale;
    float drawHeight = (float)texTree.height * treeScale;

    Vector2 treePos = {-drawWidth / 2.0f, GROUND_Y - drawHeight + 350.0f};

    // Soft drop shadow
    Vector2 shadowOffset = {moonLightDir.x * -42.0f, moonLightDir.y * -22.0f};
    DrawTexturePro(texTree, (Rectangle){0,0,(float)texTree.width,(float)texTree.height},
                    (Rectangle){treePos.x + shadowOffset.x, treePos.y + shadowOffset.y, drawWidth, drawHeight},
                    Vector2Zero(), 0, Fade(BLACK, 0.38f));

    // Main tree
    DrawTexturePro(texTree,
        (Rectangle){0, 0, (float)texTree.width, (float)texTree.height},
        (Rectangle){treePos.x, treePos.y, drawWidth, drawHeight},
        Vector2Zero(), 0.0f, WHITE);
}

void drawPlatforms(Platform_st* platforms, int count) {
    for (int i = 0; i < count; ++i) {
        Rectangle r = platforms[i].rect;

        if (platforms[i].idTex == PLATFORM_TEXTURE_WOODPLANK_ID && IsTextureValid(platformTextures[PLATFORM_TEXTURE_WOODPLANK_ID])) {
            Texture2D tex = platformTextures[PLATFORM_TEXTURE_WOODPLANK_ID];

            // Stable random wood clip
            float hash = (r.x * 13.0f + r.y * 17.0f + r.width * 19.0f);
            uint h = *(uint*)&hash;
            h = (h ^ 0xDEADBEEF) * 2654435761u;

            Rectangle source = {
                .x      = (float)(h % (uint)(tex.width - r.width  + 1)),
                .y      = (float)((h >> 16) % (uint)(tex.height - r.height + 1)),
                .width  = r.width,
                .height = r.height
            };

            // Soft drop shadow (offset opposite to moonlight)
            Vector2 shadowOffset = {moonLightDir.x * -12.0f, moonLightDir.y * -8.0f};
            DrawRectangleRec((Rectangle){r.x + shadowOffset.x, r.y + shadowOffset.y, r.width, r.height}, Fade(BLACK, 0.28f));

            // Main wood texture
            DrawTextureRec(tex, source, (Vector2){r.x, r.y}, WHITE);

            // Subtle shading on the side away from the light
            DrawRectangleRec(r, Fade(BLACK, 0.18f));   // overall dark tint
            // Light rim on the opposite side
            DrawRectangleLinesEx((Rectangle){r.x - 2, r.y - 2, r.width + 4, r.height + 4}, 3.0f, Fade(WHITE, 0.09f));
        }
    }
}

void drawWorldBoundaries(Player_st* player) {
    float limitL = -X_LIMIT;
    float limitR = X_LIMIT;

    float wallTop = -SKY_HEIGHT;
    float wallHeight = SKY_HEIGHT + GROUND_Y + 1000;
    float detectionRange = 400.0f;

    float distL = player->position.x - limitL;
    float distR = limitR - player->position.x;

    if (distL < detectionRange && distL >= 0) {
        float alphaFactor = 1.0f - (distL / detectionRange);
        unsigned char alpha = (unsigned char)(alphaFactor * 160.0f);

        DrawRectangleGradientH(limitL, wallTop, 150, wallHeight,
                               (Color){255, 255, 255, alpha},
                               (Color){255, 255, 255, 0});
        DrawLineEx((Vector2){limitL, wallTop}, (Vector2){limitL, wallTop + wallHeight}, 3.0f, (Color){255, 255, 255, alpha});
    }

    if (distR < detectionRange && distR >= 0) {
        float alphaFactor = 1.0f - (distR / detectionRange);
        unsigned char alpha = (unsigned char)(alphaFactor * 160.0f);

        DrawRectangleGradientH(limitR - 150, wallTop, 150, wallHeight,
                               (Color){255, 255, 255, 0},
                               (Color){255, 255, 255, alpha});
        DrawLineEx((Vector2){limitR, wallTop}, (Vector2){limitR, wallTop + wallHeight}, 3.0f, (Color){255, 255, 255, alpha});
    }
}