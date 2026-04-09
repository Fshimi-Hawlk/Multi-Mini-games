/**
    @file systemSettings.h
    @author Maxime-CHAUVEAU
    @author Fshimi-Hawlk
    @date 2026-02-23
    @date 2026-02-23
    @brief Settings structures for mini-games.

    Provides video and audio settings options that can be passed
    to game initialization functions. All fields have safe defaults
    when zero-initialized.

    Design principles:
      - Settings is optional (NULL = use defaults)
      - Sub-structures (video, audio) can be NULL individually
      - Macros provide default values for convenience
*/

#ifndef SYSTEM_SETTINGS_H
#define SYSTEM_SETTINGS_H

#include "baseTypes.h"
#include "APIs/generalAPI.h"

// ────────────────────────────────────────────────────────────────────────────
// Default values
// ────────────────────────────────────────────────────────────────────────────

#define DEFAULT_VIDEO_SETTING_FPS           60
#define DEFAULT_VIDEO_SETTING_WIDTH         800
#define DEFAULT_VIDEO_SETTING_HEIGHT        600
#define DEFAULT_VIDEO_SETTING_FULLSCREEN    false
#define DEFAULT_VIDEO_SETTING_VSYNC         true
#define DEFAULT_VIDEO_SETTING_BORDERLESS    true
#define DEFAULT_VIDEO_SETTING_RESIZABLE     false

#define DEFAULT_AUDIO_SETTING_VOLUME        1.0f
#define DEFAULT_AUDIO_SETTING_MUTE          false

// ────────────────────────────────────────────────────────────────────────────
// Audio settings
// ────────────────────────────────────────────────────────────────────────────

/**
    @brief Audio settings for a mini-game.

    All volume values are in range [0.0, 1.0].
*/
typedef struct {
    f32       masterVolume;   ///< Master volume multiplier
    f32       musicVolume;    ///< Background music volume
    f32       sfxVolume;      ///< Sound effects volume
    bool      mute;           ///< Mute all audio
} AudioSettings_St;

/**
    @brief Default audio settings.
*/
#define DEFAULT_AUDIO_SETTINGS (AudioSettings_St) { \
    .masterVolume = DEFAULT_AUDIO_SETTING_VOLUME, \
    .musicVolume  = DEFAULT_AUDIO_SETTING_VOLUME, \
    .sfxVolume    = DEFAULT_AUDIO_SETTING_VOLUME, \
    .mute         = DEFAULT_AUDIO_SETTING_MUTE \
}

// ────────────────────────────────────────────────────────────────────────────
// Video settings
// ────────────────────────────────────────────────────────────────────────────

/**
    @brief Video/Display settings for a mini-game.

    These are hints - the lobby may override them based on user preferences.
*/
typedef struct {
    s32         width;          ///< Window width in pixels (0 = default)
    s32         height;         ///< Window height in pixels (0 = default)
    s32         fps;            ///< Target frame rate (0 = uncapped)
    bool        fullscreen;     ///< Start in fullscreen mode
    bool        vsync;          ///< Enable vertical sync
    bool        borderless;     ///< Remove the window decorations (frame and buttons)
    bool        resizable;      ///< Enable resizing of the window
    const char* title;          ///< Window title (NULL = default game name)
} VideoSettings_St;

/**
    @brief Default video settings.
*/
#define DEFAULT_VIDEO_SETTINGS (VideoSettings_St) { \
    .fps         = DEFAULT_VIDEO_SETTING_FPS, \
    .width       = DEFAULT_VIDEO_SETTING_WIDTH, \
    .height      = DEFAULT_VIDEO_SETTING_HEIGHT, \
    .fullscreen  = DEFAULT_VIDEO_SETTING_FULLSCREEN, \
    .vsync       = DEFAULT_VIDEO_SETTING_VSYNC, \
    .borderless  = DEFAULT_VIDEO_SETTING_BORDERLESS, \
    .resizable   = DEFAULT_VIDEO_SETTING_RESIZABLE, \
    .title       = NULL \
}

// ────────────────────────────────────────────────────────────────────────────
// Main settings structure
// ────────────────────────────────────────────────────────────────────────────

/**
    @brief Combined settings for a mini-game.
*/
typedef struct {
    AudioSettings_St audio;      ///< Audio settings
    VideoSettings_St video;      ///< Video settings
} SystemSettings_St;

/**
    @brief Default game settings (all NULL = all defaults).
*/
#define DEFAULT_SYSTEM_SETTING (SystemSettings_St) { \
    .audio = DEFAULT_AUDIO_SETTINGS, \
    .video = DEFAULT_VIDEO_SETTINGS \
}

extern SystemSettings_St systemSettings;

Error_Et applySystemSettings(void);

#endif // SYSTEM_SETTINGS_H
