/**
    @file utils/configs.h
    @author Fshimi-Hawlk
    @date 2026-01-07
    @date 2026-02-23
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

// ------------------------------------------------
// Asset paths
// ------------------------------------------------

#ifndef ASSET_PATH
/**
    @brief Root directory for all game assets (textures, sounds, fonts, etc.).
           Can be overridden at compile time (e.g. -DASSET_PATH="/custom/assets/").
*/
#define ASSET_PATH "assets/"
#endif

#define IMAGES_PATH    ASSET_PATH "images/"           ///< Subdirectory containing all image files.
#define TEXTURES_PATH  ASSET_PATH "textures/"         ///< Subdirectory containing all texture files.
#define SOUNDS_PATH    ASSET_PATH "sounds/"           ///< Subdirectory containing all sound files.

// ------------------------------------------------
// Window & display
// ------------------------------------------------

#define WINDOW_WIDTH    800         ///< Default client-area width of the application window (pixels).
#define WINDOW_HEIGHT   600         ///< Default client-area height of the application window (pixels).

#define WINDOW_TITLE    "Lobby"     ///< Window title shown in the title bar / task manager.

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

// ------------------------------------------------
// Physics & movement tuning (lobby platformer)
// ------------------------------------------------

#define GRAVITY         1200.0f     ///< Downward acceleration applied every frame (pixels/second²).
#define MOVE_SPEED      300.0f      ///< Horizontal movement speed when holding left/right (pixels/second).
#define JUMP_FORCE      500.0f      ///< Upward velocity applied on jump (pixels/second).
#define GROUND_Y        500.0f      ///< Y-position of the lobby floor (matches platforms[0].rect.y).
#define COYOTE_TIME     0.1f        ///< Time window (seconds) after leaving ground where jump is still allowed.
#define JUMP_BUFFER_TIME 0.1f       ///< Time window (seconds) before landing where a pressed jump is still accepted.
#define MAX_JUMPS       2           ///< Maximum number of jumps allowed without touching ground (includes ground jump).
#define FRICTION        2000.0f     ///< Horizontal deceleration rate when no input is given (pixels/second²).

// ------------------------------------------------
// World / level geometry
// ------------------------------------------------

#define SKY_WIDTH   8000.0f     ///< Total horizontal extent of the sky/background layer.
#define SKY_HEIGHT  2000.0f     ///< Total vertical extent of the sky above the floor.
#define X_LIMIT     1600.0f     ///< World boundary: player cannot go beyond ±X_LIMIT.

// ------------------------------------------------
// Grass system
// ------------------------------------------------

#define MAX_GRASS_BLADES 80000  ///< Maximum number of simulated grass blades.

// ------------------------------------------------
// Firefly / atmospheric effects
// ------------------------------------------------

#define MAX_FIREFLIES       46  ///< Maximum number of concurrent firefly particles.
#define MAX_FALLING_LEAVES  80  ///< Maximum number of concurrent falling leaf particles.

/// Tree canopy leaf spawn zone (world-space)
#define CANOPY_CENTER_X          0.0f
#define CANOPY_CENTER_Y       -425.0f
#define CANOPY_RADIUS_MIN      800.0f
#define CANOPY_RADIUS_MAX     1200.0f
#define CANOPY_ARC_START_ANGLE -185.0f
#define CANOPY_ARC_END_ANGLE    -10.0f

/// Leaf physics tuning
#define LEAF_BASE_LIFE         42.0f
#define LEAF_GROUND_TIME        8.5f
#define LEAF_GRAVITY          265.0f
#define LEAF_PLAYER_PUSH      178.0f
#define LEAF_SPIN_DAMP_TIME     4.3f
#define LEAF_ROT_DRAG_NORMAL   0.9992f
#define LEAF_ROT_DRAG_STRONG   0.935f
#define LEAF_FLUTTER_FREQUENCY  1.5f
#define LEAF_FLUTTER_AMPLITUDE 63.0f
#define LEAF_DRIFT_FREQUENCY   0.25f
#define LEAF_DRIFT_AMPLITUDE  135.0f

#endif // CONFIGS_H
