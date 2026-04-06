#ifndef GLOBAL_H
#define GLOBAL_H

#include "constant.h"
#include "types.h"

extern Platform_st platforms[];
extern int platformCount;

extern Texture2D playerTextures[];
extern int playerTextureCount;

extern Rectangle defaultPlayerTextureRect;

extern Rectangle skinButtonRect;

extern bool isTextureMenuOpen;

extern Texture2D logoSkinButton;

extern Texture2D platformTextures[];
extern int platformTexturesCount;

extern Texture2D texTree;
extern Texture2D texBackground;

extern float gameTime;

extern GrassBlade_st grassBlades[MAX_GRASS_BLADES];
extern int grassCount;

// Moonlight direction
extern const Vector2 moonLightDir;

#endif