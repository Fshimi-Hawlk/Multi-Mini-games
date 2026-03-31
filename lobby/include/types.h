#ifndef TYPES_H
#define TYPES_H

#include "common.h"

typedef enum {
    PLATFORM_TEXTURE_GRASS_ID,
    PLATFORM_TEXTURE_WOODPLANK_ID,
} PlatformTextureId_Et;

typedef struct {
    Vector2 position;
    float radius;

    Texture2D* texture;
    float angle;

    Vector2 velocity;
    
    bool onGround;
    int nbJumps;

    float coyoteTime;
    float coyoteTimer;

    float jumpBuffer;
} Player_st;

typedef struct {
    Rectangle rect;
    PlatformTextureId_Et idTex;
} Platform_st;

typedef struct {
    Vector2 position;
    float height;
    float angle;      // L'angle actuel du brin
    float velocity;   // La vitesse de rotation (pour l'élasticité)
    Color color;
} GrassBlade_st;

#endif