#include "ui/game.h"
#include "utils/utils.h"
#include "utils/globals.h"

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
    
    // Zone de trigger pour le jeu
    DrawRectangleLinesEx(kingForFourZone, 2, GOLD);
    DrawText("KING FOR FOUR", kingForFourZone.x + 5, kingForFourZone.y + 20, 10, GOLD);
}
