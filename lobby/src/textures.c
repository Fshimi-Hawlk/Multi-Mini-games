#include "global.h"

void lobby_initTextures(void) {
    platformTextures[PLATFORM_TEXTURE_GRASS_ID]     = LoadTexture("assets/textures/grass.png");
    platformTextures[PLATFORM_TEXTURE_WOODPLANK_ID] = LoadTexture("assets/textures/wood_plank.png");

    SetTextureWrap(platformTextures[PLATFORM_TEXTURE_GRASS_ID], TEXTURE_WRAP_REPEAT);
    SetTextureWrap(platformTextures[PLATFORM_TEXTURE_WOODPLANK_ID], TEXTURE_WRAP_REPEAT);
    
    texTree = LoadTexture("assets/textures/tree.png");
    GenTextureMipmaps(&texTree);
    SetTextureFilter(texTree, TEXTURE_FILTER_BILINEAR);
    SetTextureFilter(texTree, TEXTURE_FILTER_TRILINEAR);

    texBackground = LoadTexture("assets/textures/starry-background.png");
    GenTextureMipmaps(&texBackground);
    SetTextureFilter(texBackground, TEXTURE_FILTER_BILINEAR);
    SetTextureFilter(texBackground, TEXTURE_FILTER_TRILINEAR);
}

void lobby_freeTextures(void) {
    UnloadTexture(platformTextures[PLATFORM_TEXTURE_GRASS_ID]);
    UnloadTexture(platformTextures[PLATFORM_TEXTURE_WOODPLANK_ID]);
    UnloadTexture(texTree);
    UnloadTexture(texBackground);
}