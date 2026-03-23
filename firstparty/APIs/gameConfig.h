/**
 * @file gameConfig.h
 * @brief Configuration structures for mini-games
 * @author Multi Mini-Games Team
 * @date February 2026
 */

#ifndef GAME_CONFIG_H
#define GAME_CONFIG_H

#include <stdint.h>
#include <stdbool.h>

#define GAME_DEFAULT_FPS 60
#define GAME_DEFAULT_WIDTH 1200
#define GAME_DEFAULT_HEIGHT 800
#define GAME_DEFAULT_FULLSCREEN false
#define GAME_DEFAULT_VSYNC true
#define GAME_DEFAULT_AUDIO_VOLUME 1.0f
#define GAME_DEFAULT_AUDIO_MUTE false

typedef struct {
    float masterVolume;
    float musicVolume;
    float sfxVolume;
    bool mute;
} AudioConfig_St;

typedef struct {
    int width;
    int height;
    int fps;
    bool fullscreen;
    bool vsync;
    const char* title;
} VideoConfig_St;

typedef struct {
    AudioConfig_St* audio;
    VideoConfig_St* video;
} GameConfig_St;

#define AUDIO_CONFIG_DEFAULT { \
    .masterVolume = GAME_DEFAULT_AUDIO_VOLUME, \
    .musicVolume = GAME_DEFAULT_AUDIO_VOLUME, \
    .sfxVolume = GAME_DEFAULT_AUDIO_VOLUME, \
    .mute = GAME_DEFAULT_AUDIO_MUTE \
}

#define VIDEO_CONFIG_DEFAULT { \
    .width = GAME_DEFAULT_WIDTH, \
    .height = GAME_DEFAULT_HEIGHT, \
    .fps = GAME_DEFAULT_FPS, \
    .fullscreen = GAME_DEFAULT_FULLSCREEN, \
    .vsync = GAME_DEFAULT_VSYNC, \
    .title = "Mini-Game" \
}

#define GAME_CONFIG(...) { \
    .audio = NULL, \
    .video = NULL, \
    __VA_ARGS__ \
}

#define CREATE_AUDIO_CONFIG(...) ({ \
    static AudioConfig_St _audio = AUDIO_CONFIG_DEFAULT; \
    _audio = (AudioConfig_St)AUDIO_CONFIG_DEFAULT; \
    AudioConfig_St _temp = _audio; \
    _temp = (AudioConfig_St){ __VA_ARGS__ }; \
    _audio = _temp; \
    &_audio; \
})

#define CREATE_VIDEO_CONFIG(...) ({ \
    static VideoConfig_St _video = VIDEO_CONFIG_DEFAULT; \
    _video = (VideoConfig_St)VIDEO_CONFIG_DEFAULT; \
    VideoConfig_St _temp = _video; \
    _temp = (VideoConfig_St){ __VA_ARGS__ }; \
    _video = _temp; \
    &_video; \
})

#endif // GAME_CONFIG_H
