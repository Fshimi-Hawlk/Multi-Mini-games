/**
 * @file game.c
 * @author Fshimi-Hawlk
 * @date 2026-01-30
 * @date 2026-02-18
 * @brief Low-level drawing routines for lobby gameplay elements.
 *
 * Contributors:
 * - LeandreB8:
 *    - Provided the initial drawing logic
 * - Fshimi-Hawlk:
 *    - Provided documentation
 *    - Reworked the texture logic
 *
 * This file contains primitive rendering functions used to visualize:
 *   - the player character (either debug circle or textured sprite)
 *   - static platforms in the lobby world
 *
 * These functions are meant to be called during the main render pass after
 * the camera has been set up. They operate directly on Raylib drawing primitives
 * and expect world-space coordinates.
 *
 * All functions in this file are stateless with respect to side-effects outside
 * of Raylib's drawing context - they only read game state and issue draw calls.
 *
 * @see `core/game.h` for `getPlayerCollisionBox` and `getPlayerCenter`
 * @see `utils/utils.h` for `getTextureRec`
 */

#include "ui/game.h"
#include "core/game.h"
#include "raylib.h"
#include "utils/utils.h"

void drawPlayer(const LobbyGame_St* const game, const Player_st* const player) {
    if (player->textureId == PLAYER_TEXTURE_DEFAULT) {
        DrawCircleV(player->position, player->radius, BLUE);
    }
    else {
        DrawTexturePro(
            game->playerVisuals.textures[player->textureId],
            getTextureRec(game->playerVisuals.textures[player->textureId]),
            getPlayerCollisionBox(&game->player),
            getPlayerCenter(&game->player),
            player->angle,
            WHITE
        );
    }
}

void drawPlatforms(const Platform_st* const platforms, const int nbPlatforms) {
    for (int i = 0; i < nbPlatforms; i++) {
        DrawRectangleRounded(platforms[i].rect, platforms[i].roundness, 0, platforms[i].color);
    }
}
