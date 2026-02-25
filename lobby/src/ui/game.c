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

#include "ui/game.h"

#include "utils/globals.h"
#include "utils/utils.h"

void drawPlayer(const LobbyGame_St* const game) {
    if (game->player.textureId == PLAYER_TEXTURE_DEFAULT) {
        DrawCircleV(game->player.position, game->player.radius, BLUE);
        return;
    }
    
    DrawTexturePro(
        game->playerVisuals.textures[game->player.textureId],
        getTextureRec(game->playerVisuals.textures[game->player.textureId]),  // source
        getPlayerCollisionBox(&game->player), // destination
        getPlayerCenter(&game->player), // origine du pivot
        game->player.angle, // angle en degrés
        WHITE
    );
}

void drawPlatforms(const Platform_St* const platforms, const int count) {
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

void drawTree(void) {
    if (!IsTextureValid(treeTexture)) return;

    f32 treeScale = 0.7f;
    f32 drawWidth  = (f32) treeTexture.width  * treeScale;
    f32 drawHeight = (f32) treeTexture.height * treeScale;

    Vector2 treePos = {-drawWidth / 2.0f, GROUND_Y - drawHeight + 350.0f};

    // Main tree
    DrawTexturePro(treeTexture,
        (Rectangle){0, 0, (f32) treeTexture.width, (f32) treeTexture.height},
        (Rectangle){treePos.x, treePos.y, drawWidth, drawHeight},
        Vector2Zero(), 0.0f, WHITE);
}

void drawWorldBoundaries(const Player_St* const player) {
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

