#include "global.h"

void lobby_initTextures() {
    platformTextures[PLATFORM_TEXTURE_GRASS_ID]     = LoadTexture("assets/textures/grass.png");
    platformTextures[PLATFORM_TEXTURE_WOODPLANK_ID] = LoadTexture("assets/textures/wood_plank.png");

    SetTextureWrap(platformTextures[PLATFORM_TEXTURE_GRASS_ID], TEXTURE_WRAP_REPEAT);
    SetTextureWrap(platformTextures[PLATFORM_TEXTURE_WOODPLANK_ID], TEXTURE_WRAP_REPEAT);
    
    texTree = LoadTexture("assets/textures/grok1.png");
    GenTextureMipmaps(&texTree);
    SetTextureFilter(texTree, TEXTURE_FILTER_BILINEAR);
    SetTextureFilter(texTree, TEXTURE_FILTER_TRILINEAR);
}

void lobby_freeTextures() {
    UnloadTexture(platformTextures[PLATFORM_TEXTURE_GRASS_ID]);
    UnloadTexture(platformTextures[PLATFORM_TEXTURE_WOODPLANK_ID]);
    UnloadTexture(texTree);
}