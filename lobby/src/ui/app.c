/**
 * @file app.c
 * @author i-Charlys (CAILLON Charles)
 * @date 2026-03-18
 * @brief Implementation of general UI rendering functions for the lobby application.
 */

#include "ui/app.h"
#include "utils/globals.h"
#include "utils/utils.h"

/**
 * @brief Renders the texture selection menu for player skins.
 */
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

/**
 * @brief Renders the button that opens the skin selection menu.
 */
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