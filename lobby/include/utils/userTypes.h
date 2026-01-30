/**
 * @file userTypes.h
 * @author 
 * @date 
 * @brief Core type definitions used throughout the game.
 */

#ifndef USER_TYPES_H
#define USER_TYPES_H

#include "common.h"

typedef enum {
    FONT8,
    FONT10, FONT12, FONT14, FONT16, FONT18,
    FONT20, FONT22, FONT24, FONT26, FONT28,
    FONT30, FONT32, FONT34, FONT36, FONT38,
    FONT40, FONT42, FONT44, FONT46, FONT48,
    _fontSizeCount
} FontSize_Et;

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


#endif // USER_TYPES_H