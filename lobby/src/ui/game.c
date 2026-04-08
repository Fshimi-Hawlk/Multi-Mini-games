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

#include "ui/game.h"
#include "core/game.h"
#include "utils/utils.h"
#include "utils/globals.h"
#include "utils/configs.h"

void drawPlayer(const LobbyGame_St* const game, const Player_st* const player) {
    float r = player->radius;

    // Shadow
    Vector2 shadowOffset = Vector2Scale(moonLightDir, -r * 0.45f);
    Color shadowColor = Fade(BLACK, 0.28f);

    if (player->textureId == PLAYER_TEXTURE_DEFAULT) {
        /* FIX: position IS the center — do not offset by radius */
        DrawCircleV((Vector2){player->position.x + shadowOffset.x,
                              player->position.y + shadowOffset.y}, r, shadowColor);
        DrawCircleV(player->position, r, BLUE);
    } else {
        DrawTexturePro(
            game->playerVisuals.textures[player->textureId],
            getTextureRec(game->playerVisuals.textures[player->textureId]),
            (Rectangle){player->position.x + shadowOffset.x - r,
                        player->position.y + shadowOffset.y - r, r*2, r*2},
            Vector2Zero(), player->angle, shadowColor);
        DrawTexturePro(
            game->playerVisuals.textures[player->textureId],
            getTextureRec(game->playerVisuals.textures[player->textureId]),
            getPlayerCollisionBox(player),
            getPlayerCenter(player),
            player->angle, WHITE);
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

void drawPlatforms(const Platform_st* const platforms, const int nbPlatforms) {
    for (int i = 0; i < nbPlatforms; i++) {
        DrawRectangleRounded(platforms[i].rect, platforms[i].roundness, 0, platforms[i].color);
    }
}

void drawTree(void) {
    if (!IsTextureValid(texTree)) return;

    float treeScale  = 0.7f;
    float drawWidth  = (float)texTree.width  * treeScale;
    float drawHeight = (float)texTree.height * treeScale;
    Vector2 treePos  = {-drawWidth / 2.0f, GROUND_Y - drawHeight + 350.0f};

    Vector2 shadowOffset = {moonLightDir.x * -42.0f, moonLightDir.y * -22.0f};
    DrawTexturePro(texTree,
        (Rectangle){0,0,(float)texTree.width,(float)texTree.height},
        (Rectangle){treePos.x + shadowOffset.x, treePos.y + shadowOffset.y, drawWidth, drawHeight},
        Vector2Zero(), 0, Fade(BLACK, 0.38f));

    DrawTexturePro(texTree,
        (Rectangle){0,0,(float)texTree.width,(float)texTree.height},
        (Rectangle){treePos.x, treePos.y, drawWidth, drawHeight},
        Vector2Zero(), 0.0f, WHITE);
}

void drawWorldBoundaries(const Player_st* player) {
    float limitL = -X_LIMIT, limitR = X_LIMIT;
    float wallTop = -SKY_HEIGHT;
    float wallHeight = SKY_HEIGHT + GROUND_Y + 1000;
    float detectionRange = 400.0f;

    float distL = player->position.x - limitL;
    float distR = limitR - player->position.x;

    if (distL < detectionRange && distL >= 0) {
        unsigned char alpha = (unsigned char)((1.0f - distL/detectionRange) * 160.0f);
        DrawRectangleGradientH((int)limitL, (int)wallTop, 150, (int)wallHeight,
            (Color){255,255,255,alpha}, (Color){255,255,255,0});
        DrawLineEx((Vector2){limitL,wallTop},(Vector2){limitL,wallTop+wallHeight},3.0f,(Color){255,255,255,alpha});
    }
    if (distR < detectionRange && distR >= 0) {
        unsigned char alpha = (unsigned char)((1.0f - distR/detectionRange) * 160.0f);
        DrawRectangleGradientH((int)(limitR-150), (int)wallTop, 150, (int)wallHeight,
            (Color){255,255,255,0}, (Color){255,255,255,alpha});
        DrawLineEx((Vector2){limitR,wallTop},(Vector2){limitR,wallTop+wallHeight},3.0f,(Color){255,255,255,alpha});
    }
}

