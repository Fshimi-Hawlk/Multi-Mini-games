/**
 * @file types.h
 * @author Maxime CHAUVEAU
 * @brief Basic type definitions and game constants for the bowling game.
 * 
 * This header provides platform-independent type definitions and includes
 * raylib types for 3D graphics support.
 */

#ifndef BOWLING_TYPES_H
#define BOWLING_TYPES_H

#include <raylib.h>

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;

typedef signed char s8;
typedef signed short s16;
typedef signed int s32;

typedef float f32;
typedef double f64;

#define NUM_PINS 10
#define BOWLING_MAX_FRAMES 10
#define MAX_ROLLS_PER_FRAME 2

#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720
#define TARGET_FPS 60

#endif
