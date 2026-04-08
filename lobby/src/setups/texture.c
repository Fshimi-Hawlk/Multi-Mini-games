#include "setups/app.h"
#include "utils/globals.h"

void lobby_initTextures(void) {
    logoSkinButton  = LoadTexture(IMAGES_PATH "logoSkin.png");

    game.playerVisuals.textures[PLAYER_TEXTURE_EARTH]      = LoadTexture(SKINS_PATH "earth.png");
    game.playerVisuals.textures[PLAYER_TEXTURE_TROLL_FACE] = LoadTexture(SKINS_PATH "trollFace.png");

    platformTextures[PLATFORM_TYPE_GRASS]     = LoadTexture(IMAGES_PATH "grass.png");
    platformTextures[PLATFORM_TYPE_WOODPLANK] = LoadTexture(IMAGES_PATH "wood_plank.png");

    SetTextureWrap(platformTextures[PLATFORM_TYPE_GRASS], TEXTURE_WRAP_REPEAT);
    SetTextureWrap(platformTextures[PLATFORM_TYPE_WOODPLANK], TEXTURE_WRAP_REPEAT);
    
    treeTexture = LoadTexture(IMAGES_PATH "tree.png");
    GenTextureMipmaps(&treeTexture);
    SetTextureFilter(treeTexture, TEXTURE_FILTER_BILINEAR);
    SetTextureFilter(treeTexture, TEXTURE_FILTER_TRILINEAR);

    backgroundTexture = LoadTexture(IMAGES_PATH "starry-background.png");
    GenTextureMipmaps(&backgroundTexture);
    SetTextureFilter(backgroundTexture, TEXTURE_FILTER_BILINEAR);
    SetTextureFilter(backgroundTexture, TEXTURE_FILTER_TRILINEAR);
}

void lobby_freeTextures(void) {
    if (IsTextureValid(logoSkinButton)) UnloadTexture(logoSkinButton);

    for (u8 i = 0; i < __playerTextureCount; ++i) {
        if (!IsTextureValid(game.playerVisuals.textures[i])) continue;

        UnloadTexture(game.playerVisuals.textures[i]);
    }

    for (u8 i = 0; i < __platformTypeCount; ++i) {
        if (!IsTextureValid(platformTextures[i])) continue;

        UnloadTexture(platformTextures[i]);
    }

    if (IsTextureValid(treeTexture))        UnloadTexture(treeTexture);
    if (IsTextureValid(backgroundTexture))  UnloadTexture(backgroundTexture);
}