#include "setups/texture.h"
#include "utils/globals.h"

Error_Et lobby_initTextures(Texture2D playerTextures[__playerTextureCount]) {
    Error_Et error = OK;

    const char *playerTexturePaths[__playerTextureCount] = {
        [PLAYER_TEXTURE_EARTH] = IMAGES_PATH "earth.png",
        [PLAYER_TEXTURE_TROLL_FACE] = IMAGES_PATH "trollFace.png",
    };

    for (u8 i = 0; i < __playerTextureCount; ++i) {
        if (playerTexturePaths[i] == NULL) continue;
        playerTextures[i] = LoadTexture(playerTexturePaths[i]);
        if (!IsTextureValid(playerTextures[i])) {
            error = ERROR_TEXTURE_LOAD;
        }
    }

    // Load shared UI textures
    logoSkinButton = LoadTexture(IMAGES_PATH "logoSkin.png");
    if (!IsTextureValid(logoSkinButton)) {
        log_warn("%s couldn't be loaded proprely.", IMAGES_PATH "logoSkin.png");
        error =  ERROR_TEXTURE_LOAD;
    }

    const char *platformTexturePaths[__platformTypeCount] = {
        [PLATFORM_TYPE_GRASS]     = IMAGES_PATH "grass.png",
        [PLATFORM_TYPE_WOODPLANK] = IMAGES_PATH "wood_plank.png",
    };

    for (u32 i = 0; i < __platformTypeCount; ++i) {
        platformTextures[i] = LoadTexture(platformTexturePaths[i]);
        if(!IsTextureValid(platformTextures[i])) {
            error = ERROR_TEXTURE_LOAD;
        }
    }

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

    return error;
}

void lobby_freeTextures(Texture2D playerTextures[__playerTextureCount]) {
    if (IsTextureValid(logoSkinButton)) UnloadTexture(logoSkinButton);

    for (u8 i = 0; i < __playerTextureCount; ++i) {
        if (!IsTextureValid(playerTextures[i])) continue;
        UnloadTexture(playerTextures[i]);
    }

    for (u8 i = 0; i < __platformTypeCount; ++i) {
        if (!IsTextureValid(platformTextures[i])) continue;

        UnloadTexture(platformTextures[i]);
    }

    if (IsTextureValid(treeTexture))        UnloadTexture(treeTexture);
    if (IsTextureValid(backgroundTexture))  UnloadTexture(backgroundTexture);
}