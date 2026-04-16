/**
    @file types.h
    @author Maxime CHAUVEAU
    @date 2026-02-15
    @date 2026-04-14
    @brief Basic type definitions and game constants for the bowling game.
*/
#ifndef BOWLING_TYPES_H
#define BOWLING_TYPES_H

#include <raylib.h>

typedef unsigned char u8;    ///< 8-bit unsigned integer
typedef unsigned short u16;  ///< 16-bit unsigned integer
typedef unsigned int u32;    ///< 32-bit unsigned integer

typedef signed char s8;      ///< 8-bit signed integer
typedef signed short s16;    ///< 16-bit signed integer
typedef signed int s32;      ///< 32-bit signed integer

typedef float f32;           ///< 32-bit float
typedef double f64;          ///< 64-bit float

/**
    @brief Number of pins in the triangle formation.
*/
#define NUM_PINS 10

/**
    @brief Total number of frames in a complete game.
*/
#define BOWLING_MAX_FRAMES 10

/**
    @brief Maximum number of rolls allowed per frame (except for the 10th frame).
*/
#define MAX_ROLLS_PER_FRAME 2

/**
    @brief Default screen width in pixels.
*/
#define SCREEN_WIDTH 1280

/**
    @brief Default screen height in pixels.
*/
#define SCREEN_HEIGHT 720

/**
    @brief Target frames per second.
*/
#define TARGET_FPS 60

#endif
