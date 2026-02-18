#include "ui/game.h"
#include "utils/utils.h"

void drawPlayer(const LobbyGame_St* const game) {
    if (game->player.visuals.textureId == PLAYER_TEXTURE_DEFAULT) {
        DrawCircleV(game->player.position, game->player.radius, BLUE);
        return;
    }
    
    DrawTexturePro(
        game->player.visuals.textures[game->player.visuals.textureId],
        getTextureRec(game->player.visuals.textures[game->player.visuals.textureId]),  // source
        getPlayerCollisionBox(&game->player), // destination
        getPlayerCenter(&game->player), // origine du pivot
        game->player.angle, // angle en degrés
        WHITE
    );
}

void drawPlatforms(const Platform_st* const platforms, const int nbPlatforms) {
    for (int i = 0; i < nbPlatforms; i++)
        DrawRectangleRounded(platforms[i].rect, platforms[i].roundness, 0, platforms[i].color);
}
