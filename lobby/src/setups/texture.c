#include "setups/texture.h"
#include "utils/globals.h"

Error_Et lobby_initTextures(Texture2D playerTextures[__playerTextureCount]) {
    Error_Et error = OK;

    const char *playerTexturePaths[__playerTextureCount] = {
        [PLAYER_TEXTURE_BINGO] =            SKINS_PATH "bingo.png",
        [PLAYER_TEXTURE_BOWLING] =          SKINS_PATH "bowling.png",
        [PLAYER_TEXTURE_CHESS] =            SKINS_PATH "todo.png",
        [PLAYER_TEXTURE_DROP_FOUR] =        SKINS_PATH "todo.png",
        [PLAYER_TEXTURE_TWIST_CUBE] =       SKINS_PATH "todo.png",
        [PLAYER_TEXTURE_EDITOR] =           SKINS_PATH "todo.png",
        [PLAYER_TEXTURE_KING_FOR_FOUR] =    SKINS_PATH "king.png",
        [PLAYER_TEXTURE_LOBBY] =            SKINS_PATH "earth.png",
        [PLAYER_TEXTURE_MINI_GOLF] =        SKINS_PATH "todo.png",
        [PLAYER_TEXTURE_DISC_REVERSAL] =    SKINS_PATH "todo.png",
        [PLAYER_TEXTURE_POLY_BLAST] =       SKINS_PATH "todo.png",
        [PLAYER_TEXTURE_SNAKE] =            SKINS_PATH "snake.png",
        [PLAYER_TEXTURE_SOLO_CARDS] =       SKINS_PATH "todo.png",
        [PLAYER_TEXTURE_SUIKA] =            SKINS_PATH "suika.png",
        [PLAYER_TEXTURE_TETROMINO_FALL] =   SKINS_PATH "todo.png",
    };

    for (u8 i = 0; i < __playerTextureCount; ++i) {
        if (playerTexturePaths[i] == NULL) continue;
        playerTextures[i] = LoadTexture(playerTexturePaths[i]);
        if (!IsTextureValid(playerTextures[i])) {
            error = ERROR_TEXTURE_LOAD;
        }
    }

    // Load shared UI textures
    logoSkinButton = LoadTexture(TEXTURES_PATH "logoSkin.png");
    if (!IsTextureValid(logoSkinButton)) {
        log_warn("%s couldn't be loaded proprely.", TEXTURES_PATH "logoSkin.png");
        error =  ERROR_TEXTURE_LOAD;
    }

    const char *platformTexturePaths[__terrainKindCount] = {
        [TERRAIN_KIND_GRASS]     = TEXTURES_PATH "grass.png",
        [TERRAIN_KIND_WOOD_PLANK] = TEXTURES_PATH "wood_plank.png",
    };

    for (u32 i = 0; i < __terrainKindCount; ++i) {
        if (platformTexturePaths[i] == NULL) continue;
        terrainTextures[i] = LoadTexture(platformTexturePaths[i]);
        if(!IsTextureValid(terrainTextures[i])) {
            error = ERROR_TEXTURE_LOAD;
        }
    }

    SetTextureWrap(terrainTextures[TERRAIN_KIND_GRASS], TEXTURE_WRAP_REPEAT);
    SetTextureWrap(terrainTextures[TERRAIN_KIND_WOOD_PLANK], TEXTURE_WRAP_REPEAT);
    
    treeTexture = LoadTexture(TEXTURES_PATH "tree.png");
    GenTextureMipmaps(&treeTexture);
    SetTextureFilter(treeTexture, TEXTURE_FILTER_BILINEAR);
    SetTextureFilter(treeTexture, TEXTURE_FILTER_TRILINEAR);

    backgroundTexture = LoadTexture(TEXTURES_PATH "starry-background.png");
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

    for (u8 i = 0; i < __terrainKindCount; ++i) {
        if (!IsTextureValid(terrainTextures[i])) continue;

        UnloadTexture(terrainTextures[i]);
    }

    if (IsTextureValid(treeTexture))        UnloadTexture(treeTexture);
    if (IsTextureValid(backgroundTexture))  UnloadTexture(backgroundTexture);
}