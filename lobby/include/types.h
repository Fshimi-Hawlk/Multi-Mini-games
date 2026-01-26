#ifndef TYPES_H
#define TYPES_H

#include "common.h"

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
    Color color;
    float roundness;
} Platform_st;

#endif