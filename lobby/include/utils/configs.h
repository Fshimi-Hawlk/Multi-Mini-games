/**
 * @file configs.h
 * @author i-Charlys (CAILLON Charles)
 * @date 2026-03-18
 * @brief Core configuration constants for the game.
 */

#ifndef CONFIGS_H
#define CONFIGS_H

#ifndef ASSET_PATH
/**
 * @brief Base path for all game assets.
 */
#define ASSET_PATH "assets/"
#endif

/**
 * @brief Path for images assets.
 */
#define IMAGES_PATH ASSET_PATH "images/"

/**
 * @brief Default window width.
 */
#define WINDOW_WIDTH    1000

/**
 * @brief Default window height.
 */
#define WINDOW_HEIGHT   600

/**
 * @brief Application window title.
 */
#define WINDOW_TITLE    "Lobby"

/**
 * @brief Background color of the application.
 */
#define APP_BACKGROUND_COLOR (color32) {.r = 18, .g = 18, .b = 18, .a = 255}

/**
 * @brief Base text size for the application.
 */
#define APP_TEXT_FONT_SIZE 32

/**
 * @brief Gravity constant for physics.
 */
#define GRAVITY 1200.0f

/**
 * @brief Player movement speed.
 */
#define MOVE_SPEED 300.0f

/**
 * @brief Player jump force.
 */
#define JUMP_FORCE 500.0f

/**
 * @brief Ground Y level offset.
 */
#define GROUND_Y 50

/**
 * @brief Coyote time duration in seconds.
 */
#define COYOTE_TIME 0.1f

/**
 * @brief Jump buffer time duration in seconds.
 */
#define JUMP_BUFFER_TIME 0.1f

/**
 * @brief Maximum number of jumps allowed (e.g., double jump).
 */
#define MAX_JUMPS 2

/**
 * @brief Friction constant for movement deceleration.
 */
#define FRICTION 2000

#endif // CONFIGS_H