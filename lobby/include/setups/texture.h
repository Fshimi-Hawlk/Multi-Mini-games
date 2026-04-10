#ifndef UI_TEXTURE_H
#define UI_TEXTURE_H

#include "utils/userTypes.h"

Error_Et lobby_initTextures(Texture2D playerTextures[__playerTextureCount]);
void lobby_freeTextures(Texture2D playerTextures[__playerTextureCount]);

#endif // UI_TEXTURE_H