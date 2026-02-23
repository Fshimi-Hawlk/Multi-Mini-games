/**
    @file utils/globals.c
    @author Fshimi-Hawlk
    @date 2026-01-25
    @date 2026-02-23
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

// TODO