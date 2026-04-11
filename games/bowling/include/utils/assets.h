#ifndef BOWLING_ASSETS_H
#define BOWLING_ASSETS_H

#include <raylib.h>
#include "utils/types.h"
#include "APIs/generalAPI.h"

#define MAX_BALL_TEXTURES 16

typedef struct {
    Texture2D ballTextures[MAX_BALL_TEXTURES];
    int       ballTextureCount;
    Model     ballModels[MAX_BALL_TEXTURES];       // sphère texturée 3D par skin
    int       ballModelCount;
    Color     ballCenterColors[MAX_BALL_TEXTURES]; // couleur dominante pour fond de sphère
    Texture2D pinTexture;
    Texture2D laneTexture;
    Model     crowdModel;
    bool      crowdModelLoaded;
} BowlingTextures_St;

Error_Et bowling_loadTextures(BowlingTextures_St* textures);
void     bowling_unloadTextures(BowlingTextures_St* textures);
void     bowling_fillBallTextures(BowlingTextures_St* textures);

#endif
