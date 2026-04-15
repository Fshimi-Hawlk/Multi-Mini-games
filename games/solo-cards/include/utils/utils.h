/**
    @file utils.h
    @author Fshimi-Hawlk
    @date 2026-02-06
    @date 2026-04-14
    @brief Utility function declarations for color management in the Solo Cards game.
*/
#ifndef UTILS_UTILS_H
#define UTILS_UTILS_H

#include "utils/common.h"

/**
    @brief Compares two Raylib Color structures for equality.
    @param[in]     c1           First color to compare.
    @param[in]     c2           Second color to compare.
    @return                     0 if colors are identical, non-zero otherwise.
*/
bool compareColor(Color c1, Color c2);

/**
    @brief Checks if two Raylib colors are identical.
    @param[in]     c1           First color to check.
    @param[in]     c2           Second color to check.
    @return                     true if colors are equal, false otherwise.
*/
bool isColorsEqual(Color c1, Color c2);

#endif
