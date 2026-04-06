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

typedef enum {
    FIREFLY_MODE_WANDER,
    FIREFLY_MODE_LOOP,
    FIREFLY_MODE_BOB
} FireflyMode_Et;

typedef struct {
    Vector2 position;
    Vector2 velocity;
    float   radius;
    float   alpha;
    float   phase;
    bool    active;

    // Enhanced behavior
    FireflyMode_Et mode;
    float          modeTimer;
    Vector2        wanderTarget;
    Vector2        loopPoints[12];
    int            loopCount;
    int            currentLoopIndex;
    float          facingAngle;
    float          currentSpeed;
} Firefly_St;

typedef struct {
    Vector2 position;
    Vector2 velocity;
    float   rotation;
    float   rotationSpeed;
    float   scale;
    float   life;
    float   currentAlpha;
    bool    active;
    bool    onGround;
    float   groundTimer;
    float   spinDampTimer;      ///< Time left to apply strong rotational drag after player push (0 = normal drag)
    Color   color;
} FallingLeaf_St;

#endif