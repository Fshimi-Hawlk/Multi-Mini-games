#ifndef GLOBAL_H
#define GLOBAL_H

#include "types.h"

// --- CONFIGURATION DU LOBBY ---
#define WINDOW_WIDTH  800
#define WINDOW_HEIGHT 600
#define GROUND_Y      500
#define PLAT_H        20
#define STEP_Y        100
#define WALL_W        100
#define DOOR_H        200

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

extern float gameTime;

#define MAX_GRASS_BLADES 80000
extern GrassBlade_st grassBlades[MAX_GRASS_BLADES];
extern int grassCount;

#endif