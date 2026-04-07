/**
    @file systemSettings.h
    @author Maxime-CHAUVEAU
    @author Fshimi-Hawlk
    @date 2026-02-23
    @brief Settings structures for mini-games.
*/

#ifndef SYSTEM_SETTINGS_H
#define SYSTEM_SETTINGS_H

#include "baseTypes.h"
#include "APIs/generalAPI.h"

// ----------------------------------------------------------------------------
// Default values
// ----------------------------------------------------------------------------

#define DEFAULT_VIDEO_SETTING_FPS           60
#define DEFAULT_VIDEO_SETTING_WIDTH         1200
#define DEFAULT_VIDEO_SETTING_HEIGHT        800
#define DEFAULT_VIDEO_SETTING_FULLSCREEN    false
#define DEFAULT_VIDEO_SETTING_VSYNC         true
#define DEFAULT_VIDEO_SETTING_BORDERLESS    true
#define DEFAULT_VIDEO_SETTING_RESIZABLE     false

#define DEFAULT_AUDIO_SETTING_VOLUME        1.0f
#define DEFAULT_AUDIO_SETTING_MUTE          false

// ----------------------------------------------------------------------------
// Audio settings
// ----------------------------------------------------------------------------

typedef struct {
    f32       masterVolume;
    f32       musicVolume;
    f32       sfxVolume;
    bool      mute;
} AudioSettings_St;

#define DEFAULT_AUDIO_SETTINGS (AudioSettings_St) { \
    .masterVolume = DEFAULT_AUDIO_SETTING_VOLUME, \
    .musicVolume  = DEFAULT_AUDIO_SETTING_VOLUME, \
    .sfxVolume    = DEFAULT_AUDIO_SETTING_VOLUME, \
    .mute         = DEFAULT_AUDIO_SETTING_MUTE \
}

// ----------------------------------------------------------------------------
// Video settings
// ----------------------------------------------------------------------------

typedef struct {
    s32         width;
    s32         height;
    s32         fps;
    bool        fullscreen;
    bool        vsync;
    bool        borderless;
    bool        resizable;
    const char* title;
} VideoSettings_St;

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

// ----------------------------------------------------------------------------
// Main settings structure
// ----------------------------------------------------------------------------

typedef struct {
    AudioSettings_St audio;
    VideoSettings_St video;
} SystemSettings_St;

#define DEFAULT_SYSTEM_SETTING (SystemSettings_St) { \
    .audio = DEFAULT_AUDIO_SETTINGS, \
    .video = DEFAULT_VIDEO_SETTINGS \
}

#ifndef SYSTEM_SETTINGS_IMPLEMENTATION
extern SystemSettings_St systemSettings;
Error_Et applySystemSettings(void);
#else
SystemSettings_St systemSettings = {0};

static Error_Et applyAudioSettings(AudioSettings_St settings) {
    Error_Et err = OK;

    if (settings.mute != 0 && settings.mute != 1) {
        log_warn("Received `mute` setting exceed value range (0 or 1)");
        err = ERROR_INVALID_SETTING;
        settings.mute = DEFAULT_AUDIO_SETTING_MUTE;
    }

    if (0.0f <= settings.masterVolume && settings.masterVolume <= 1.0f) {
        SetMasterVolume(settings.mute * settings.masterVolume);
    } else {
        log_warn("Received `masterVolume` setting exceed value range [0; 1]");
        err = ERROR_INVALID_SETTING;
    }

    return err;
}

static Error_Et applyVideoSettings(VideoSettings_St settings) {
    Error_Et err = OK;

    if (settings.title != NULL) {
        SetWindowTitle(settings.title);
    } else {
        log_warn("Received `title` setting was set to NULL");
        err = ERROR_INVALID_SETTING;
    }

    if (settings.fps > 0) {
        SetTargetFPS(settings.fps);
    } else {
        log_warn("Received `FPS` setting was set to zero");
        err = ERROR_INVALID_SETTING;
    }

    if (settings.width > 0 && settings.height > 0) {
        SetWindowSize(settings.width, settings.height);
    } else {
        log_warn("Received `width` and/or `height` settings was set to zero");
        err = ERROR_INVALID_SETTING;
    }

    if (settings.fullscreen != 0 && settings.fullscreen != 1) {
        log_warn("Received `fullscreen` setting exceed value range (0 or 1)");
        err = ERROR_INVALID_SETTING;
        settings.fullscreen = DEFAULT_VIDEO_SETTING_FULLSCREEN;
    }

    if (settings.vsync != 0 && settings.vsync != 1) {
        log_warn("Received `vsync` setting exceed value range (0 or 1)");
        err = ERROR_INVALID_SETTING;
        settings.vsync = DEFAULT_VIDEO_SETTING_VSYNC;
    }

    if (settings.borderless != 0 && settings.borderless != 1) {
        log_warn("Received `borderless` setting exceed value range (0 or 1)");
        err = ERROR_INVALID_SETTING;
        settings.borderless = DEFAULT_VIDEO_SETTING_BORDERLESS;
    }

    if (settings.resizable != 0 && settings.resizable != 1) {
        log_warn("Received `resizable` setting exceed value range (0 or 1)");
        err = ERROR_INVALID_SETTING;
        settings.resizable = DEFAULT_VIDEO_SETTING_RESIZABLE;
    }

    SetWindowState(
           (settings.fullscreen ? FLAG_FULLSCREEN_MODE : 0)
        |  (settings.vsync      ? FLAG_VSYNC_HINT : 0)
        |  (settings.borderless ? FLAG_WINDOW_UNDECORATED : 0)
        |  (settings.resizable  ? FLAG_WINDOW_RESIZABLE : 0)
    );

    return err;
}

Error_Et applySystemSettings(void) {
    Error_Et err = OK;
    
    err = applyVideoSettings(systemSettings.video);
    err = applyAudioSettings(systemSettings.audio);

    return err;
}
#endif

#endif // SYSTEM_SETTINGS_H