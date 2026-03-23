/**
 * @file configs.h
 * @author Maxime CHAUVEAU
 * @brief Asset path configurations for the bowling game.
 * 
 * This header defines all file paths for game assets including
 * textures.
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
#define ASSETS_PATH   "assets/"  ///< Root asset directory. Override at compile time if needed (e.g. -DASSETS_PATH='"../bowling/assets/"').
#endif
#define BALL_TEXTURE  ASSETS_PATH "boules.png"
#define PIN_TEXTURE   ASSETS_PATH "quilles.png"
#define LANE_TEXTURE  ASSETS_PATH "piste.png"
#define CROWD_MODEL   ASSETS_PATH "crowd.obj"

#ifndef SOUNDS_PATH
#define SOUNDS_PATH ASSETS_PATH "sounds/"
#endif

#endif