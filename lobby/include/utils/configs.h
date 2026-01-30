/**
 * @file configs.h
 * @author 
 * @date 
 * @brief Core configuration constants for the game.
 */

#ifndef CONFIGS_H
#define CONFIGS_H

#define WINDOW_TITLE "Template"
#define WINDOW_WIDTH 1000
#define WINDOW_HEIGHT 600

#define APP_BACKGROUND_COLOR (color32) {.r = 18, .g = 18, .b = 18, .a = 255}

#define APP_TEXT_FONT_SIZE 32       ///< Base text size

// Game Constants
#define GRAVITY 1200.0f
#define MOVE_SPEED 300.0f
#define JUMP_FORCE 500.0f
#define GROUND_Y 50

#define COYOTE_TIME 0.1f
#define JUMP_BUFFER_TIME 0.1f
#define MAX_JUMPS 2

#define FRICTION 2000

#endif // CONFIGS_H