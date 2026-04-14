/**
    @file globals.c
    @author Kimi BERGE
    @date 2026-01-25
    @date 2026-04-14
    @brief Global variable definitions for the Bingo game.
*/
#include "utils/globals.h"

// General Variables
Font bingo_fonts[__fontSizeCount] = {0}; ///< Array of preloaded fonts for various UI sizes.

// Game's Variables
const char* const LETTERS[5] = {"B", "I", "N", "G", "O"}; ///< Bingo column letters.
