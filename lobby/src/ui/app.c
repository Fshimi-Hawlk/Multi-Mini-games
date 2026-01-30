#include "ui/app.h"
#include "utils/globals.h"
#include "utils/utils.h"

void drawMenuTextures(void) {
    Rectangle destRect = defaultPlayerTextureRect;

    DrawText("choose your skin :", 20, 40, 20, DARKGRAY);
    for (int i = 0; i < playerTextureCount; i++) {
        destRect.x = 20 + i * 60;
        DrawTexturePro(
            playerTextures[i],
            getTextureRec(&playerTextures[i]), // source
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
        getTextureRec(&logoSkinButton),
        skinButtonRect,
        Vector2Zero(),
        0,
        WHITE
    );
}