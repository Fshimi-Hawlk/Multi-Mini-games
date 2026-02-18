#include "ui/app.h"
#include "utils/globals.h"
#include "utils/utils.h"

void drawMenuTextures(const LobbyGame_St* const game) {
    Rectangle destRect = game->player.visuals.defaultTextureRect;

    DrawText("choose your skin :", 20, 40, 20, DARKGRAY);
    for (int i = 0; i < __playerTextureCount; i++) {
        destRect.x = 20 + i * 60;
        DrawTexturePro(
            game->player.visuals.textures[i],
            getTextureRec(game->player.visuals.textures[i]), // source
            destRect, // destination
            Vector2Zero(),
            0,
            WHITE
        );
    }
}

void drawSkinButton(void) {
    DrawTexturePro(
        logoSkinButton,
        getTextureRec(logoSkinButton),
        skinButtonRect,
        Vector2Zero(),
        0,
        WHITE
    );
}