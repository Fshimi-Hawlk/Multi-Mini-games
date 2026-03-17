/**
    @file utils/configs.h
    @author Maxime CHAUVEAU
    @date 2026-02-25
    @brief Central place for compile-time and tuning constants used throughout the game.

    This header defines:
        - Paths to asset directories
        - Window / display settings
        - Visual defaults (colors, font sizes)
        - Core gameplay tuning values (physics, movement, jump mechanics)

    All values here are intended to be easily tweakable without touching logic code.
    When a constant is used in many places or affects feel significantly, it belongs here.
*/

#ifndef CONFIGS_H
#define CONFIGS_H

// ────────────────────────────────────────────────
// Asset paths
// ────────────────────────────────────────────────

#ifndef ASSET_PATH
/**
    @brief Root directory for all game assets (textures, sounds, fonts, etc.).
           Can be overridden at compile time (e.g. -DASSET_PATH="/custom/assets/")
 */
#define ASSET_PATH "assets/"
#endif

#define IMAGES_PATH ASSET_PATH "images/"            ///< Subdirectory containing all image files.

// ────────────────────────────────────────────────
// Window & display
// ────────────────────────────────────────────────

#define WINDOW_WIDTH    1280        ///< Default client-area width of the application window (pixels).
#define WINDOW_HEIGHT   720         ///< Default client-area height of the application window (pixels).

#define WINDOW_TITLE    "Multi-Mini-Games Lobby"     ///< Window title shown in the title bar / task manager.

/**
    @brief Background clear color used at the start of each frame.
           Dark gray (#121212).
 */
#define APP_BACKGROUND_COLOR ((Color){18, 18, 18, 255})

/**
    @brief Default base font size for most UI text and in-game labels.
           Other sizes are usually derived from this via the fonts[] array.
 */
#define APP_TEXT_FONT_SIZE 32

// ────────────────────────────────────────────────
// Physics & movement tuning (lobby platformer)
// ────────────────────────────────────────────────

#define GRAVITY         1200.0f     ///< Downward acceleration applied every frame (pixels/second²).
#define MOVE_SPEED      300.0f      ///< Horizontal movement speed when holding left/right (pixels/second).
#define JUMP_FORCE      500.0f      ///< Upward velocity applied on jump (pixels/second).
#define GROUND_Y        50.0f       ///< Y-position considered "ground level" for initial spawn / debug.
#define COYOTE_TIME     0.1f        ///< Time window (seconds) after leaving ground where jump is still allowed.
#define JUMP_BUFFER_TIME 0.1f       ///< Time window (seconds) before landing where a pressed jump is still accepted.
#define MAX_JUMPS       2           ///< Maximum number of jumps allowed without touching ground (includes ground jump).
#define FRICTION        2000.0f     ///< Horizontal deceleration rate when no input is given (pixels/second²).

// ────────────────────────────────────────────────
// Game zone settings
// ────────────────────────────────────────────────

#define GAME_ZONE_BOWLING_X       600
#define GAME_ZONE_BOWLING_Y       -150
#define GAME_ZONE_BOWLING_WIDTH   100
#define GAME_ZONE_BOWLING_HEIGHT  100

#define GAME_ZONE_PROXIMITY_THRESHOLD 150.0f  ///< Distance at which game zones start highlighting

// ────────────────────────────────────────────────
// Bowling integration settings
// ────────────────────────────────────────────────

#define BOWLING_SCREEN_WIDTH  1280
#define BOWLING_SCREEN_HEIGHT 720

#endif // CONFIGS_H
