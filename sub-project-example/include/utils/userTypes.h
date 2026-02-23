/**
    @file userTypes.h
    @author Fshimi-Hawlk
    @date 2026-01-25
    @date 2026-02-23
    @brief Core type definitions used throughout the game.

    Contributors:
        - Fshimi-Hawlk:
            - Added documentation start-up
            - Added `FontSize_Et`

    If the file needs more context than fits in @brief, write 2-5 lines here.
    @note Put warnings, important limitations, "we know it's ugly but...", or future plans here

    // Try to align the `for` for better readability
    // Try to keep the same order of the includes
    Use @see `path/to/related/file.h` when this file depends heavily on another one.
*/

#ifndef USER_TYPES_H
#define USER_TYPES_H

#include "common.h"

/**
    @brief Available font sizes used for in-game UI and text rendering.

    Values are listed in ascending order.  
    `_fontSizeCount` is **not** a valid font size - it serves as array dimension / loop boundary.
*/
typedef enum {
    FONT8,
    FONT10, FONT12, FONT14, FONT16, FONT18,
    FONT20, FONT22, FONT24, FONT26, FONT28,
    FONT30, FONT32, FONT34, FONT36, FONT38,
    FONT40, FONT42, FONT44, FONT46, FONT48,
    _fontSizeCount
} FontSize_Et;


// Game's Types

// TODO

#endif // USER_TYPES_H