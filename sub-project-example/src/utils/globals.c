/**
 * @file globals.c
 * @author 
 * @date 
 * @brief Definitions of global variables.
 */

#include "utils/globals.h"
#include "utils/userTypes.h"

Rectangle windowRect = {0, 0, WINDOW_WIDTH, WINDOW_HEIGHT};
Font      fonts[_fontSizeCount] = {0};
Font      appFont = {0};
