#include "global.h"

#include "skin.h"

void drawMenuTextures(void) {
    Rectangle destRect = defaultPlayerTextureRect;

    DrawText("choose your skin :", 20, 40, 20, DARKGRAY);
    for (int i = 0; i < playerTextureCount; i++) {
        destRect.x = 20 + i * 60;
        DrawTexturePro(
            playerTextures[i],
            (Rectangle){0, 0, playerTextures[i].width, playerTextures[i].height}, // source
            destRect, // destination
            (Vector2){0, 0},
            0,
            WHITE
        );
    }
}

void choosePlayerTexture(Player_st* player) {
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        Vector2 mousePos = GetMousePosition();
        Rectangle destRect = defaultPlayerTextureRect;

        for (int i = 0; i < playerTextureCount; i++) {
            destRect.x = 20 + i * 60;
            if (CheckCollisionPointRec(mousePos, destRect)) {
                player->texture = &playerTextures[i];
                isTextureMenuOpen = false;
                break;
            }
        }
    }
    if (IsKeyPressed(KEY_ONE)) {
        player->texture = &playerTextures[0];
        isTextureMenuOpen = false;
    }
    if (IsKeyPressed(KEY_TWO)) {
        player->texture = &playerTextures[1];
        isTextureMenuOpen = false;
    }
}

void drawSkinButton(void) {
    DrawTexturePro(
        logoSkinButton,
        (Rectangle){0, 0, logoSkinButton.width, logoSkinButton.height},
        skinButtonRect,
        (Vector2){0, 0},
        0,
        WHITE
    );
}

void toggleSkinMenu(void) {
    if ((IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && CheckCollisionPointRec(GetMousePosition(), skinButtonRect)) || IsKeyPressed(KEY_P)) {
        isTextureMenuOpen = !isTextureMenuOpen;
    }
}