#include "setups/texture.h"
#include "utils/globals.h"

void lobby_initTextures(LobbyGame_St* const game) {
    logoSkinButton  = LoadTexture(IMAGES_PATH "logoSkin.png");

    const char *playerTexturePaths[__playerTextureCount] = {
        [PLAYER_TEXTURE_EARTH]      = SKINS_PATH "earth.png",
        [PLAYER_TEXTURE_TROLL_FACE] = SKINS_PATH "trollFace.png",
    };

    for (u8 i = 0; i < __playerTextureCount; ++i) {
        if (playerTexturePaths[i] == NULL) continue;
        game->playerVisuals.textures[i] = LoadTexture(playerTexturePaths[i]);
        if (!IsTextureValid(game->playerVisuals.textures[i])) {
            log_warn("Couldn't load player skin: %s", playerTexturePaths[i]);
        }
    }

    const char *platformTexturePaths[__platformTypeCount] = {
        [PLATFORM_TYPE_GRASS]     = IMAGES_PATH "grass.png",
        [PLATFORM_TYPE_WOODPLANK] = IMAGES_PATH "wood_plank.png",
    };

    for (u8 i = 0; i < __platformTypeCount; ++i) {
        if (platformTexturePaths[i] == NULL) continue;
        platformTextures[i] = LoadTexture(platformTexturePaths[i]);
        if (!IsTextureValid(platformTextures[i])) {
            log_warn("Couldn't load player skin: %s", platformTexturePaths[i]);
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
}

void lobby_freeTextures(LobbyGame_St* const game) {
    if (IsTextureValid(logoSkinButton)) UnloadTexture(logoSkinButton);

    for (u8 i = 0; i < __playerTextureCount; ++i) {
        if (!IsTextureValid(game->playerVisuals.textures[i])) continue;

        UnloadTexture(game->playerVisuals.textures[i]);
    }

    for (u8 i = 0; i < __platformTypeCount; ++i) {
        if (!IsTextureValid(platformTextures[i])) continue;

        UnloadTexture(platformTextures[i]);
    }

    if (IsTextureValid(treeTexture))        UnloadTexture(treeTexture);
    if (IsTextureValid(backgroundTexture))  UnloadTexture(backgroundTexture);
}