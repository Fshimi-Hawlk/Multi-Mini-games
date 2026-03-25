/**
    @file utils/configs.h
    @author Fshimi-Hawlk
    @date 2026-01-07
    @date 2026-03-20
    @brief Central place for compile-time and tuning constants used throughout the game.

    This header defines:
        - Paths to asset directories
        - Window / display settings
        - Visual defaults (colors, font sizes)
        - Core gameplay tuning values (physics, movement, jump mechanics)

    All values here are intended to be easily tweakable without touching logic code.
    When a constant is used in many places or affects feel significantly, it belongs here.

    Guidelines:
        - Prefer named constants over magic numbers in .c files
        - Use this file for values that are unlikely to change per build/environment
        - For runtime-configurable settings (e.g. via file or menu), use globals or a config struct instead
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
