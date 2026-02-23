/**
 * @file gameConfig.h
 * @author Maxime-CHAUVEAU
 * @date 2026-02-23
 * @brief Configuration structures for mini-games.
 *
 * Provides video and audio configuration options that can be passed
 * to game initialization functions. All fields have safe defaults
 * when zero-initialized.
 *
 * Design principles:
 *   - Configuration is optional (NULL = use defaults)
 *   - Sub-structures (video, audio) can be NULL individually
 *   - Macros provide default values for convenience
 *
 * @see generalAPI.h for error codes
 */

#ifndef GAME_CONFIG_H
#define GAME_CONFIG_H

#include <stdint.h>

// ────────────────────────────────────────────────────────────────────────────
// Default values
// ────────────────────────────────────────────────────────────────────────────

#define GAME_DEFAULT_FPS           60
#define GAME_DEFAULT_WIDTH         1200
#define GAME_DEFAULT_HEIGHT        800
#define GAME_DEFAULT_FULLSCREEN    false
#define GAME_DEFAULT_VSYNC         true
#define GAME_DEFAULT_VOLUME        1.0f
#define GAME_DEFAULT_MUTE          false

// ────────────────────────────────────────────────────────────────────────────
// Audio configuration
// ────────────────────────────────────────────────────────────────────────────

/**
 * @brief Audio settings for a mini-game.
 *
 * All volume values are in range [0.0, 1.0].
 */
typedef struct {
    float       masterVolume;   ///< Master volume multiplier
    float       musicVolume;    ///< Background music volume
    float       sfxVolume;      ///< Sound effects volume
    bool        mute;           ///< Mute all audio
} AudioConfig_St;

/**
 * @brief Default audio configuration.
 */
#define AUDIO_CONFIG_DEFAULT { \
    .masterVolume = GAME_DEFAULT_VOLUME, \
    .musicVolume  = GAME_DEFAULT_VOLUME, \
    .sfxVolume    = GAME_DEFAULT_VOLUME, \
    .mute         = GAME_DEFAULT_MUTE \
}

// ────────────────────────────────────────────────────────────────────────────
// Video configuration
// ────────────────────────────────────────────────────────────────────────────

/**
 * @brief Video/Display settings for a mini-game.
 *
 * These are hints - the lobby may override them based on user preferences.
 */
typedef struct {
    int         width;          ///< Window width in pixels (0 = default)
    int         height;         ///< Window height in pixels (0 = default)
    int         fps;            ///< Target frame rate (0 = uncapped)
    bool        fullscreen;     ///< Start in fullscreen mode
    bool        vsync;          ///< Enable vertical sync
    const char* title;          ///< Window title (NULL = default game name)
} VideoConfig_St;

/**
 * @brief Default video configuration.
 */
#define VIDEO_CONFIG_DEFAULT { \
    .width       = GAME_DEFAULT_WIDTH, \
    .height      = GAME_DEFAULT_HEIGHT, \
    .fps         = GAME_DEFAULT_FPS, \
    .fullscreen  = GAME_DEFAULT_FULLSCREEN, \
    .vsync       = GAME_DEFAULT_VSYNC, \
    .title       = NULL \
}

// ────────────────────────────────────────────────────────────────────────────
// Main configuration structure
// ────────────────────────────────────────────────────────────────────────────

/**
 * @brief Combined configuration for a mini-game.
 *
 * Both pointers may be NULL - games should use defaults in that case.
 * Games should check each pointer before accessing its fields.
 */
typedef struct {
    AudioConfig_St* audio;      ///< Audio settings (NULL = defaults)
    VideoConfig_St* video;      ///< Video settings (NULL = defaults)
} GameConfig_St;

/**
 * @brief Default game configuration (all NULL = all defaults).
 */
#define GAME_CONFIG_DEFAULT { \
    .audio = NULL, \
    .video = NULL \
}

// ────────────────────────────────────────────────────────────────────────────
// Helper macros for inline configuration
// ────────────────────────────────────────────────────────────────────────────

/**
 * @brief Create an inline video config with custom values.
 *
 * Example:
 *   VideoConfig_St* video = CREATE_VIDEO_CONFIG(.fps = 120, .fullscreen = true);
 */
#define CREATE_VIDEO_CONFIG(...) ({ \
    static VideoConfig_St _cfg = VIDEO_CONFIG_DEFAULT; \
    _cfg = (VideoConfig_St)VIDEO_CONFIG_DEFAULT; \
    VideoConfig_St _tmp = _cfg; \
    _tmp = (VideoConfig_St){ __VA_ARGS__ }; \
    _cfg = _tmp; \
    &_cfg; \
})

/**
 * @brief Create an inline audio config with custom values.
 *
 * Example:
 *   AudioConfig_St* audio = CREATE_AUDIO_CONFIG(.mute = true);
 */
#define CREATE_AUDIO_CONFIG(...) ({ \
    static AudioConfig_St _cfg = AUDIO_CONFIG_DEFAULT; \
    _cfg = (AudioConfig_St)AUDIO_CONFIG_DEFAULT; \
    AudioConfig_St _tmp = _cfg; \
    _tmp = (AudioConfig_St){ __VA_ARGS__ }; \
    _cfg = _tmp; \
    &_cfg; \
})

#endif // GAME_CONFIG_H
