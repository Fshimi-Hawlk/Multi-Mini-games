/**
 * @file configs.h
 * @author Fshimi-Hawlk
 * @date 2026-01-07
 * @date 2026-02-20
 * @brief Central place for compile-time and tuning constants used throughout the game.
 *
 * Contributors:
 * - Fshimi-Hawlk:
 *    - Added documentation start-up
 *    - Added base configs
 *
 * This header defines:
 *   - Paths to asset directories
 *   - Window / display settings
 *   - Visual defaults (colors, font sizes)
 *   - <Game's configs>
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
 * @brief Subdirectory containing all image files.
 */
#define IMAGES_PATH ASSET_PATH "images/"

// ────────────────────────────────────────────────
// Window & display
// ────────────────────────────────────────────────

/**
 * @brief Default client-area width of the application window (pixels).
 */
#define WINDOW_WIDTH    1000

/**
 * @brief Default client-area height of the application window (pixels).
 */
#define WINDOW_HEIGHT   600

/**
 * @brief Window title shown in the title bar / task manager.
 */
#define WINDOW_TITLE    "GameName"

/**
 * @brief Background clear color used at the start of each frame.
 *        Dark gray (#121212).
 */
#define APP_BACKGROUND_COLOR ((Color) {18, 18, 18, 255})

/**
 * @brief Default base font size for most UI text and in-game labels.
 *        Other sizes are usually derived from this via the fonts[] array.
 */
#define APP_TEXT_FONT_SIZE 32

// ────────────────────────────────────────────────
// Game Configs
// ────────────────────────────────────────────────



#endif // CONFIGS_H