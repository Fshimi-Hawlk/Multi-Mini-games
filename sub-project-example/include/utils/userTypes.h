/**
 * @file userTypes.h
 * @author Fshimi-Hawlk
 * @date 2026-01-07
 * @date 2026-02-20
 * @brief Core type definitions used throughout the game.
 *
 * Contributors:
 * - Fshimi-Hawlk:
 *    - Added documentation start-up
 *    - Added `FontSize_Et`
 *
 * Longer description if needed (2–5 lines max):
 *   - What this file/module does
 *   - Key types/functions it exposes
 *   - Important constraints or usage rules
 *   - Cross-references to related files (@see ...)
 *
 * @note Any noteworthy limitation, future plan or non-obvious decision
 * @see `modulePath/moduleName` for [[`stuff that comes from the module`], ...]
 */

#ifndef USER_TYPES_H
#define USER_TYPES_H

#include "common.h"
#include "APIs/generalAPI.h"

/**
 * @brief Available font sizes used for in-game UI and text rendering.
 *
 * Values are listed in ascending order.  
 * `_fontSizeCount` is **not** a valid font size - it serves as array dimension / loop boundary.
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