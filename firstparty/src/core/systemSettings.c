/**
    @file systemSettings.c
    @author Multi Mini-Games Team
    @date 2026-04-14
    @date 2026-04-14
    @brief systemSettings.c implementation/header file
*/
#include "systemSettings.h"
#include "logger.h"

SystemSettings_St systemSettings = {
    .audio = {
        .masterVolume = DEFAULT_AUDIO_SETTING_VOLUME,
        .musicVolume  = DEFAULT_AUDIO_SETTING_VOLUME,
        .sfxVolume    = DEFAULT_AUDIO_SETTING_VOLUME,
        .mute         = DEFAULT_AUDIO_SETTING_MUTE
    },
    .video = {
        .fps         = DEFAULT_VIDEO_SETTING_FPS,
        .width       = DEFAULT_VIDEO_SETTING_WIDTH,
        .height      = DEFAULT_VIDEO_SETTING_HEIGHT,
        .fullscreen  = DEFAULT_VIDEO_SETTING_FULLSCREEN,
        .vsync       = DEFAULT_VIDEO_SETTING_VSYNC,
        .borderless  = DEFAULT_VIDEO_SETTING_BORDERLESS,
        .resizable   = DEFAULT_VIDEO_SETTING_RESIZABLE,
        .title       = NULL
    }
};

static Error_Et applyAudioSettings(AudioSettings_St settings) {
    Error_Et err = OK;

    if (settings.mute != 0 && settings.mute != 1) {
        log_warn("Received `mute` setting exceed value range (0 or 1)");
        err = ERROR_INVALID_SETTING;
        settings.mute = DEFAULT_AUDIO_SETTING_MUTE;
    }

    if (0.0f <= settings.masterVolume && settings.masterVolume <= 1.0f) {
        SetMasterVolume(settings.mute ? 0.0f : settings.masterVolume);
    } else {
        log_warn("Received `masterVolume` setting exceed value range [0; 1]");
        err = ERROR_INVALID_SETTING;
    }

    // if (0.0f <= settings.musicVolume && settings.musicVolume <= 1.0f) {
    //     SetMusicVolume(settings.musicVolume); // Note: Need `music`
    // } else {
    //     log_warn("Received `musicVolume` setting exceed value range [0; 1]");
    //     err = ERROR_INVALID_SETTING;
    // }

    // if (0.0f <= settings.sfxVolume && settings.sfxVolume <= 1.0f) {
    //     SetSoundVolume(settings.sfxVolume); // Note: Need `sound`
    // } else {
    //     log_warn("Received `sfxVolume` setting exceed value range [0; 1]");
    //     err = ERROR_INVALID_SETTING;
    // }

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

    // Validate dimensions
    if (settings.width <= 0 || settings.height <= 0) {
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

    // 1. Toggle fullscreen FIRST — SetWindowSize must come after so the windowed
    //    size is applied in the correct mode.
    // On Wayland, ToggleFullscreen() tries to change monitor resolution (X11 behaviour)
    // and is silently ignored. ToggleBorderlessWindowed() uses the xdg-toplevel fullscreen
    // protocol and works correctly on both Wayland and X11.
    bool isFullscreenNow = IsWindowState(FLAG_BORDERLESS_WINDOWED_MODE) || IsWindowFullscreen();
    if ((bool)settings.fullscreen != isFullscreenNow) {
        ToggleBorderlessWindowed();
    }

    // 2. Resize only when windowed and the stored size differs from current.
    bool isFullscreenAfter = IsWindowState(FLAG_BORDERLESS_WINDOWED_MODE) || IsWindowFullscreen();
    if (settings.width > 0 && settings.height > 0) {
        if (!isFullscreenAfter && (GetScreenWidth() != settings.width || GetScreenHeight() != settings.height)) {
            SetWindowSize(settings.width, settings.height);
        }
    }

    // 3. Apply vsync regardless of fullscreen state.
    if (settings.vsync) SetWindowState(FLAG_VSYNC_HINT); else ClearWindowState(FLAG_VSYNC_HINT);

    // 4. Decoration and resize hints are irrelevant in fullscreen and can confuse
    //    the compositor.
    if (!isFullscreenAfter) {
        if (settings.borderless) SetWindowState(FLAG_WINDOW_UNDECORATED); else ClearWindowState(FLAG_WINDOW_UNDECORATED);
        if (settings.resizable)  SetWindowState(FLAG_WINDOW_RESIZABLE);   else ClearWindowState(FLAG_WINDOW_RESIZABLE);
    }

    return err;
}

Error_Et applySystemSettings(void) {
    Error_Et err = OK;
    
    err = applyVideoSettings(systemSettings.video);
    err = applyAudioSettings(systemSettings.audio);

    return err;
}