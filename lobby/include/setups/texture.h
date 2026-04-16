/**
    @file texture.h
    @author Léandre BAUDET
    @date 2026-04-14
    @date 2026-04-14
    @brief texture.h implementation/header file
*/
#ifndef UI_TEXTURE_H
#define UI_TEXTURE_H

#include "utils/userTypes.h"

/**
    @brief Description for lobby_initTextures
    @param[in,out] playerTextures[__playerTextureCount] The playerTextures[__playerTextureCount] parameter
    @return Success/failure or the result of the function
*/
Error_Et lobby_initTextures(Texture2D playerTextures[__playerTextureCount]);
/**
    @brief Description for lobby_freeTextures
    @param[in,out] playerTextures[__playerTextureCount] The playerTextures[__playerTextureCount] parameter
*/
void lobby_freeTextures(Texture2D playerTextures[__playerTextureCount]);

#endif // UI_TEXTURE_H