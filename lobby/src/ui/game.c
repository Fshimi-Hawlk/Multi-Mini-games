/**
    @file ui/game.c
    @author Fshimi-Hawlk
    @date 2026-01-30
    @date 2026-02-23
    @brief Low-level drawing routines for lobby gameplay elements.

    Contributors:
        - LeandreB8:
            - Provided the initial drawing logic
        - Fshimi-Hawlk:
            - Provided documentation
            - Reworked the texture logic

    This file contains primitive rendering functions used to visualize:
        - the player character (either debug circle or textured sprite)
        - static platforms in the lobby world

    These functions are meant to be called during the main render pass after
    the camera has been set up. They operate directly on Raylib drawing primitives
    and expect world-space coordinates.

    All functions in this file are stateless with respect to side-effects outside
    of Raylib's drawing context - they only read game state and issue draw calls.

    @see `core/game.h` for `getPlayerCollisionBox` and `getPlayerCenter`
    @see `utils/utils.h` for `getTextureRec`
*/

#include "core/game.h"
#include "APIs/generalAPI.h"
#include "baseTypes.h"
#include "ui/game.h"

#include "utils/globals.h"

#include "sharedUtils/geometry.h"
#include "utils/userTypes.h"

void lobby_drawPlayer(const PlayerVisuals_St* const visuals, const Player_St* const player) {
    float r = player->radius;

    // Shadow
    Vector2 shadowOffset = Vector2Scale(moonLightDir, -r * 0.45f);
    Color shadowColor = Fade(BLACK, 0.28f);

    DrawCircleV((Vector2) {
            player->position.x + shadowOffset.x,
            player->position.y + shadowOffset.y
        }, r, shadowColor
    );

    if (player->textureId == PLAYER_TEXTURE_DEFAULT) {
        DrawCircleV(player->position, r, BLUE);
    } else {
        DrawTexturePro(
            visuals->textures[player->textureId],
            getTextureRec(visuals->textures[player->textureId]),
            lobby_getPlayerCollisionBox(player),
            lobby_getPlayerCenter(player),
            player->angle, WHITE
        );
    }

    // Moonlight glow
    Vector2 glowPos = {
        player->position.x + moonLightDir.x * r * 0.38f,
        player->position.y + moonLightDir.y * r * 0.38f
    };
    Color glowBase = (Color){180, 220, 255, 255};
    DrawCircleV(glowPos, r * 0.72f, Fade(glowBase, 0.09f));
    DrawCircleV(glowPos, r * 0.48f, Fade(glowBase, 0.14f));
    DrawCircleV(glowPos, r * 0.26f, Fade(glowBase, 0.11f));
}

void lobby_drawPlatforms(const Platform_St* const platforms, const int count) {
    for (int i = 0; i < count; ++i) {
        const Platform_St* p = &platforms[i];
        Rectangle r = p->rect;

        if (p->type == PLATFORM_TYPE_WOODPLANK && IsTextureValid(platformTextures[PLATFORM_TYPE_WOODPLANK])) {
            Texture2D tex = platformTextures[PLATFORM_TYPE_WOODPLANK];

            // Stable random wood clip
            f32 hash = (r.x * 13.0f + r.y * 17.0f + r.width * 19.0f);
            uint h = *(uint*)&hash;
            h = (h ^ 0xDEADBEEF) * 2654435761u;

            Rectangle source = {
                .x      = (f32)(h % (uint)(tex.width - r.width  + 1)),
                .y      = (f32)((h >> 16) % (uint)(tex.height - r.height + 1)),
                .width  = r.width,
                .height = r.height
            };

            // Soft drop shadow
            Vector2 shadowOffset = {moonLightDir.x * -12.0f, moonLightDir.y * -8.0f};
            DrawRectangleRec((Rectangle){r.x + shadowOffset.x, r.y + shadowOffset.y, r.width, r.height}, Fade(BLACK, 0.28f));

            // Main wood texture
            DrawTextureRec(tex, source, (Vector2){r.x, r.y}, WHITE);

            // Subtle shading
            DrawRectangleRec(r, Fade(BLACK, 0.18f));
            // Light rim
            DrawRectangleLinesEx((Rectangle){r.x - 2, r.y - 2, r.width + 4, r.height + 4}, 3.0f, Fade(WHITE, 0.09f));
        }
        // Add grass or other solid types here later if needed
    }
}

// void drawLobbyTerrains(void) {
//     // FPS Optimization: Pre-calculate some values
//     float time = (float)GetTime();
    
//     for (u32 i = 0; i < terrains.count; i++) {
//         const LobbyTerrain_St* t = &terrains.items[i];
        
//         // Visibility check (basic culling)
//         // Note: For simplicity we assume they are somewhat near the player
        
//         switch (t->type) {
//             case TERRAIN_WATER: {
//                 Color waterColor = (Color){ 30, 120, 250, 140 };
//                 DrawRectangleRec(t->rect, waterColor);
//                 // Simple wave effect (no Rounded call)
//                 float wave = sinf(time * 2.0f + t->rect.x * 0.01f) * 4.0f;
//                 DrawLineEx((Vector2){t->rect.x, t->rect.y + wave}, (Vector2){t->rect.x + t->rect.width, t->rect.y + wave}, 3.0f, (Color){150, 220, 255, 200});
//             } break;

//             case TERRAIN_BOUNCY:
//                 DrawRectangleRec(t->rect, (Color){255, 50, 255, 255});
//                 DrawRectangleLinesEx(t->rect, 2.0f, WHITE);
//                 break;

//             case TERRAIN_ICE:
//                 DrawRectangleRec(t->rect, (Color){200, 240, 255, 255});
//                 DrawRectangleLinesEx(t->rect, 1.0f, Fade(WHITE, 0.5f));
//                 break;

//             case TERRAIN_PORTAL: {
//                 float pulse = (sinf(time * 5.0f) + 1.0f) * 0.5f;
//                 DrawRectangleRec(t->rect, (Color){100, 0, 200, 200});
//                 DrawRectangleLinesEx(t->rect, 2.0f + pulse * 3.0f, PURPLE);
//                 DrawCircleV(getRectCenterPos(t->rect), 5 + pulse * 10, WHITE);
//             } break;

//             case TERRAIN_WOOD:
//                 DrawRectangleRec(t->rect, (Color){100, 60, 20, 255});
//                 DrawRectangleLinesEx(t->rect, 1.0f, (Color){60, 30, 10, 255});
//                 break;

//             case TERRAIN_STONE:
//                 DrawRectangleRec(t->rect, (Color){120, 120, 120, 255});
//                 DrawRectangleLinesEx(t->rect, 1.5f, DARKGRAY);
//                 break;

//             case TERRAIN_DECORATIVE:
//                 // Use Rounded ONLY for decoration to save FPS on static platforms
//                 DrawRectangleRounded(t->rect, t->roundness, 6, t->color);
//                 break;

//             default: // TERRAIN_NORMAL
//                 DrawRectangleRec(t->rect, t->color);
//                 DrawRectangleLinesEx(t->rect, 1.0f, Fade(BLACK, 0.2f));
//                 break;
//         }
//     }
// }

void lobby_drawTree(void) {
    if (!IsTextureValid(treeTexture)) return;

    f32 treeScale = 0.7f;
    f32 drawWidth  = (f32) treeTexture.width  * treeScale;
    f32 drawHeight = (f32) treeTexture.height * treeScale;

    Vector2 treePos = {-drawWidth / 2.0f, GROUND_Y - drawHeight + 350.0f};

    Vector2 shadowOffset = {moonLightDir.x * -42.0f, moonLightDir.y * -22.0f};
    DrawTexturePro(treeTexture,
        (Rectangle){0,0,(float)treeTexture.width,(float)treeTexture.height},
        (Rectangle){treePos.x + shadowOffset.x, treePos.y + shadowOffset.y, drawWidth, drawHeight},
        Vector2Zero(), 0, Fade(BLACK, 0.38f));


    // Main tree
    DrawTexturePro(treeTexture,
        (Rectangle){0, 0, (f32) treeTexture.width, (f32) treeTexture.height},
        (Rectangle){treePos.x, treePos.y, drawWidth, drawHeight},
        Vector2Zero(), 0.0f, WHITE);
}

void lobby_drawWorldBoundaries(const Player_St* const player) {
    f32 limitL = -X_LIMIT;
    f32 limitR = X_LIMIT;

    f32 wallTop = -SKY_HEIGHT;
    f32 wallHeight = SKY_HEIGHT + GROUND_Y + 1000;
    f32 detectionRange = 400.0f;

    f32 distL = player->position.x - limitL;
    f32 distR = limitR - player->position.x;

    if (distL < detectionRange && distL >= 0) {
        f32 alphaFactor = 1.0f - (distL / detectionRange);
        unsigned char alpha = (unsigned char)(alphaFactor * 160.0f);

        DrawRectangleGradientH(limitL, wallTop, 150, wallHeight,
                               (Color){255, 255, 255, alpha},
                               (Color){255, 255, 255, 0});
        DrawLineEx((Vector2){limitL, wallTop}, (Vector2){limitL, wallTop + wallHeight}, 3.0f, (Color){255, 255, 255, alpha});
    }

    if (distR < detectionRange && distR >= 0) {
        f32 alphaFactor = 1.0f - (distR / detectionRange);
        unsigned char alpha = (unsigned char)(alphaFactor * 160.0f);

        DrawRectangleGradientH(limitR - 150, wallTop, 150, wallHeight,
                               (Color){255, 255, 255, 0},
                               (Color){255, 255, 255, alpha});
        DrawLineEx((Vector2){limitR, wallTop}, (Vector2){limitR, wallTop + wallHeight}, 3.0f, (Color){255, 255, 255, alpha});
    }
}

void lobby_drawGameZones(const Player_St* const player) {
    for (u8 i = 1; i < __miniGameIdCount; ++i) {
        if (i == MINI_GAME_ID_LOBBY) continue;

        GameInteractionZone_St gameZone = gameZones[i];
        if (gameZone.name == NULL) continue;
        if (gameZone.hitbox.width == 0) continue; 

        bool playerNear = CheckCollisionCircleRec(
            player->position, player->radius, gameZone.hitbox);
        
        // Glowing effect for game zones
        float glow = (sinf(time(NULL) * 3.0f) + 1.0f) * 0.5f;
        Color edgeColor = playerNear ? GOLD : WHITE;

        DrawRectangleRounded(gameZone.hitbox, 0.3f, 8, gameZone.color);
        DrawRectangleRoundedLinesEx(gameZone.hitbox, 0.3f, 8, 2.0f + glow * 2.0f, edgeColor);

        f32 cx = gameZone.hitbox.x + gameZone.hitbox.width / 2.0f;
        f32 cy = gameZone.hitbox.y + gameZone.hitbox.height / 2.0f;
        DrawTriangle(
            (Vector2){cx,        cy - 18},
            (Vector2){cx - 12,   cy + 8},
            (Vector2){cx + 12,   cy + 8},
            WHITE);

        const char* gameName = gameZone.name;

        f32Vector2 nameSize = MeasureTextEx(lobby_fonts[FONT24], gameName, 22, 0);
        f32Vector2 namePosition = {
            .x = gameZone.hitbox.x + (gameZone.hitbox.width - nameSize.x) / 2.0f,
            .y = gameZone.hitbox.y - 30.0f - glow * 5.0f
        };

        DrawTextEx(lobby_fonts[FONT24], gameName, namePosition, nameSize.y, 0, edgeColor);

        /* "[ E ]" en dessous si proche */
        if (playerNear) {
            const char* prompt = "[ E ]";
            f32Vector2 promptSize = MeasureTextEx(lobby_fonts[FONT16], prompt, 16, 0);
            DrawTextEx(lobby_fonts[FONT32], prompt, (Vector2) {
                    gameZone.hitbox.x + (gameZone.hitbox.width - promptSize.x) / 2.0f,
                    gameZone.hitbox.y + gameZone.hitbox.height + 4
                }, 16, 0, YELLOW
            );
        }
    }
}
