/**
    @file assets.h
    @author Maxime CHAUVEAU
    @date 2026-04-07
    @date 2026-04-14
    @brief Asset management for the Bowling mini-game.
*/
#ifndef BOWLING_ASSETS_H
#define BOWLING_ASSETS_H

#include <raylib.h>
#include "utils/types.h"
#include "APIs/generalAPI.h"

/**
    @brief Maximum number of ball textures that can be extracted from the sprite sheet.
*/
#define MAX_BALL_TEXTURES 16

/**
    @brief Container for all textures and models used in the bowling game.
*/
typedef struct {
    Texture2D ballTextures[MAX_BALL_TEXTURES];      ///< Extracted ball skin textures
    int       ballTextureCount;                     ///< Number of valid textures found
    Model     ballModels[MAX_BALL_TEXTURES];        ///< 3D sphere models per skin
    int       ballModelCount;                       ///< Number of loaded models
    Color     ballCenterColors[MAX_BALL_TEXTURES];  ///< Dominant color for each ball skin
    Texture2D pinTexture;                           ///< Texture for bowling pins
    Texture2D laneTexture;                          ///< Texture for the bowling lane
    Model     crowdModel;                           ///< 3D model for the audience
    bool      crowdModelLoaded;                     ///< true if the crowd model was loaded successfully
} BowlingTextures_St;

/**
    @brief Loads all required textures and models for the bowling game.

    @param[out] textures    Texture container to fill
    @return                 OK on success, or an error code on failure
*/
Error_Et bowling_loadTextures(BowlingTextures_St* textures);

/**
    @brief Unloads all textures and models to free memory.

    @param[in,out] textures    Texture container to clean up
*/
void bowling_unloadTextures(BowlingTextures_St* textures);

/**
    @brief Procedurally fills ball textures with colors for a better look.

    @param[in,out] textures    Texture container
*/
void bowling_fillBallTextures(BowlingTextures_St* textures);

#endif // BOWLING_ASSETS_H
