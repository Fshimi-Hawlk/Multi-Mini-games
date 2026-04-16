/**
    @file configs.h
    @author Maxime CHAUVEAU
    @date 2026-02-01
    @date 2026-04-14
    @brief Configuration constants for the Suika mini-game.
*/
#ifndef SUIKA_UTILS_CONFIGS_H
#define SUIKA_UTILS_CONFIGS_H

/** @brief Screen width in pixels */
#define SUIKA_SCREEN_WIDTH  800

/** @brief Screen height in pixels */
#define SUIKA_SCREEN_HEIGHT 900

/** @brief Maximum number of fruits that can exist simultaneously */
#define SUIKA_MAX_FRUITS        128

/** @brief Width of the fruit container in pixels */
#define SUIKA_CONTAINER_WIDTH   600

/** @brief Height of the fruit container in pixels */
#define SUIKA_CONTAINER_HEIGHT  700

/** @brief X position of the container left edge */
#define SUIKA_CONTAINER_X       100

/** @brief Y position of the container top edge */
#define SUIKA_CONTAINER_Y       150

/** @brief Y position of the game over line - fruits above this cause game over */
#define SUIKA_DROP_LINE_Y       200

/**
    @brief Helper macro to create a Color from RGB values (alpha = 255)

    @param r Red component (0-255)
    @param g Green component (0-255)
    @param b Blue component (0-255)
    @return Color structure
*/
#define RGB(r, g, b) (Color) {r, g, b, 255}

/**
    @brief Sound assets path — overridable at compile time via -DASSET_PATH.
*/
#ifdef ASSET_PATH
#  define SOUNDS_PATH ASSET_PATH "sounds/"
#else
#  define SOUNDS_PATH "assets/sounds/"
#endif

#endif
