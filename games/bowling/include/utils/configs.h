/**
    @file configs.h
    @author Maxime CHAUVEAU
    @date 2026-04-07
    @date 2026-04-14
    @brief Asset path configurations for the bowling game.
*/
#ifndef BOWLING_CONFIGS_H
#define BOWLING_CONFIGS_H

#include "types.h"

// Allow root Makefile override via -DASSET_PATH
#ifdef ASSET_PATH
#  ifndef ASSETS_PATH
#    define ASSETS_PATH ASSET_PATH
#  endif
#endif

#ifndef ASSETS_PATH
/**
    @brief Root asset directory. Override at compile time if needed (e.g. -DASSETS_PATH='"../bowling/assets/"').
*/
#define ASSETS_PATH   "assets/"
#endif

/**
    @brief Texture sprite sheet for bowling balls.
*/
#define BALL_TEXTURE  ASSETS_PATH "boules.png"

/**
    @brief Texture for bowling pins.
*/
#define PIN_TEXTURE   ASSETS_PATH "quilles.png"

/**
    @brief Texture for the bowling lane surface.
*/
#define LANE_TEXTURE  ASSETS_PATH "piste.png"

/**
    @brief 3D OBJ model for the audience/crowd.
*/
#define CROWD_MODEL   ASSETS_PATH "crowd.obj"

#ifndef SOUNDS_PATH
/**
    @brief Root directory for game sound effects.
*/
#define SOUNDS_PATH ASSETS_PATH "sounds/"
#endif

#endif // BOWLING_CONFIGS_H
