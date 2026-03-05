/**
    @file utils/globals.c
    @author Fshimi-Hawlk
    @date 2026-01-25
    @date 2026-03-05
    @brief Definitions of program-wide global variables.

    Contributors:
        - Fshimi-Hawlk:
            - Provided documentation
        - <Name>:
            - ...

    Contains shared state and constants used across the application:
        - Window rectangle
        - Font handles

    @see `utils/globals.h` for full documentation and extern declarations
*/

#include "utils/globals.h"

// General Variables

Rectangle windowRect = {0, 0, WINDOW_WIDTH, WINDOW_HEIGHT};
Font      fonts[_fontSizeCount] = {0};
Font      appFont = {0};

// Game's Variables

const char* const LETTERS[5] = {"B", "I", "N", "G", "O"};

const f32 SHOW_DELAY   = 1.5f;
const f32 GRACE_TIME   = 1.0f;
const f32 CHOICE_DELAY = 3.5f;