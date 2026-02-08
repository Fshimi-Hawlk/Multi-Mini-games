#include "ui/game.h"
#include "utils/utils.h"

void drawPlayer(const Player_st* const player) {
    if (player->texture == NULL) {
        DrawCircleV(player->position, player->radius, BLUE);
        return;
    }
    
    DrawTexturePro(
        *player->texture,
        getTextureRec(player->texture),  // source
        getPlayerCollisionBox(player), // destination
        getPlayerCenter(player), // origine du pivot
        player->angle, // angle en degrés
        WHITE
    );
}

void drawPlatforms(const Platform_st* const platforms, const int nbPlatforms) {
    for (int i = 0; i < nbPlatforms; i++)
        DrawRectangleRounded(platforms[i].rect, platforms[i].roundness, 0, platforms[i].color);
}
