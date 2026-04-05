#ifndef GLOBAL_H
#define GLOBAL_H

#include "types.h"

// --- CONFIGURATION DU LOBBY ---
#define WINDOW_WIDTH  800.0f
#define WINDOW_HEIGHT 600.0f
#define GROUND_Y      500.0f
#define PLAT_H        20.0f
#define STEP_Y        100.0f
#define WALL_W        100.0f
#define DOOR_H        200.0f

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