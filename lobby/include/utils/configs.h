/**
    @file utils/configs.h
    @author Fshimi-Hawlk
    @date 2026-01-07
    @date 2026-03-30
    @brief Central place for compile-time and tuning constants used throughout the game.
*/

#ifndef CONFIGS_H
#define CONFIGS_H

#ifndef ASSET_PATH
/**
    @brief Root directory for all game assets.
*/
#define ASSET_PATH "assets/"
#endif

#define FONT_PATH ASSET_PATH "fonts/"
#define IMAGES_PATH ASSET_PATH "images/"

#define WINDOW_TITLE "Multi-Mini-Games"
#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 720

#define APP_BACKGROUND_COLOR (Color) { 18, 18, 18, 255 }

#define APP_TEXT_FONT_SIZE 32       ///< Base text size

// ────────────────────────────────────────────────
// Physics & movement tuning (lobby platformer)
// ────────────────────────────────────────────────

#define GRAVITY             1200.0f
#define MOVE_SPEED          300.0f
#define JUMP_FORCE         -500.0f
#define GROUND_Y            50.0f
#define COYOTE_TIME         0.1f
#define JUMP_BUFFER_TIME    0.1f
#define MAX_JUMPS           2
#define FRICTION            2000.0f

// ────────────────────────────────────────────────
// Water terrain physics
// ────────────────────────────────────────────────

#define WATER_BUOYANCY      -320.0f
#define WATER_HORIZ_DRAG     0.82f
#define WATER_VERT_DRAG      0.88f
#define WATER_JUMP_FORCE    -320.0f

// ────────────────────────────────────────────────
// Ice terrain physics
// ────────────────────────────────────────────────

#define ICE_FRICTION        120.0f

#endif // CONFIGS_H
