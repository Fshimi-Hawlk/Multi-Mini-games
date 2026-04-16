/**
    @file utils.c
    @author Fshimi-Hawlk
    @date 2026-02-06
    @date 2026-04-14
    @brief Utility function implementations for color comparison within the Solo Cards game.
*/
#include "utils/utils.h"

bool compareColor(Color c1, Color c2) {
    return memcmp(&c1, &c2, sizeof(Color));
}

bool isColorsEqual(Color c1, Color c2) {
    return compareColor(c1, c2) == 0;
}