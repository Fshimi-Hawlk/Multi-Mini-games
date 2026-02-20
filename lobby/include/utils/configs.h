/**
 * @file configs.h
 * @author Fshimi-Hawlk
 * @date 2026-01-07
 * @date 2026-02-18
 * @brief Central place for compile-time and tuning constants used throughout the game.
 *
 * This header defines:
 *   - Paths to asset directories
 *   - Window / display settings
 *   - Visual defaults (colors, font sizes)
 *   - Core gameplay tuning values (physics, movement, jump mechanics)
 *
 * All values here are intended to be easily tweakable without touching logic code.
 * When a constant is used in many places or affects feel significantly, it belongs here.
 *
 * Guidelines:
 *   - Prefer named constants over magic numbers in .c files
 *   - Use this file for values that are unlikely to change per build/environment
 *   - For runtime-configurable settings (e.g. via file or menu), use globals or a config struct instead
 */

#ifndef CONFIGS_H
#define CONFIGS_H

// ────────────────────────────────────────────────
// Asset paths
// ────────────────────────────────────────────────

#ifndef ASSET_PATH
/**
 * @brief Root directory for all game assets (textures, sounds, fonts, etc.).
 *        Can be overridden at compile time (e.g. -DASSET_PATH="/custom/assets/").
 */
#define ASSET_PATH "assets/"
#endif

/**
 * @brief 
 */
#define IMAGES_PATH ASSET_PATH "images/"            ///< Subdirectory containing all image files.

// ────────────────────────────────────────────────
// Window & display
// ────────────────────────────────────────────────

#define WINDOW_WIDTH    1000        ///< Default client-area width of the application window (pixels).
#define WINDOW_HEIGHT   600         ///< Default client-area height of the application window (pixels).

#define WINDOW_TITLE    "Lobby"     ///< Window title shown in the title bar / task manager.

/**
 * @brief Background clear color used at the start of each frame.
 *        Dark gray (#121212).
 */
#define APP_BACKGROUND_COLOR ((Color){18, 18, 18, 255})

/**
 * @brief Default base font size for most UI text and in-game labels.
 *        Other sizes are usually derived from this via the fonts[] array.
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

#endif // CONFIGS_H