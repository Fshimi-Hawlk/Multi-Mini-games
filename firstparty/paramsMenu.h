/**
 * @file paramsMenu.h
 * @brief Shared parameters menu UI – gear button + settings panel
 * @author Multi Mini-Games Team
 * @date 2026-03-28
 *
 * Three sections: Resolution, Audio (master volume + mute), Display (FPS counter).
 *
 * Usage:
 *   1. Define PARAMS_MENU_IMPLEMENTATION in exactly one .c file before including
 *   2. Include raylib.h BEFORE this header when implementation is enabled
 *   3. Include this header normally in all other files
 */

#ifndef PARAMS_MENU_H
#define PARAMS_MENU_H

#include "baseTypes.h"
#include "raylib.h"

// ────────────────────────────────────────────────----------------------------
// Resolution options
// ────────────────────────────────────────────────----------------------------

typedef enum {
    RESOLUTION_800x600 = 0,
    RESOLUTION_1024x768,
    RESOLUTION_1280x720,
    RESOLUTION_1366x768,
    RESOLUTION_1920x1080,
    RESOLUTION_COUNT
} Resolution_Et;

static inline s32 paramsMenu_getWidth(Resolution_Et res) {
    switch (res) {
        case RESOLUTION_800x600:   return 800;
        case RESOLUTION_1024x768:  return 1024;
        case RESOLUTION_1280x720:  return 1280;
        case RESOLUTION_1366x768:  return 1366;
        case RESOLUTION_1920x1080: return 1920;
        default: return 800;
    }
}

static inline s32 paramsMenu_getHeight(Resolution_Et res) {
    switch (res) {
        case RESOLUTION_800x600:   return 600;
        case RESOLUTION_1024x768:  return 768;
        case RESOLUTION_1280x720:  return 720;
        case RESOLUTION_1366x768:  return 768;
        case RESOLUTION_1920x1080: return 1080;
        default: return 600;
    }
}

static inline const char* paramsMenu_getLabel(Resolution_Et res) {
    switch (res) {
        case RESOLUTION_800x600:   return "800 x 600";
        case RESOLUTION_1024x768:  return "1024 x 768";
        case RESOLUTION_1280x720:  return "1280 x 720";
        case RESOLUTION_1366x768:  return "1366 x 768";
        case RESOLUTION_1920x1080: return "1920 x 1080";
        default: return "800 x 600";
    }
}

// ────────────────────────────────────────────────----------------------------
// Menu state
// ────────────────────────────────────────────────----------------------------

typedef struct {
    bool          isOpen;         ///< Whether the settings panel is open
    Resolution_Et selected;       ///< Currently active resolution

    Texture2D     gearTexture;    ///< Gear icon texture
    bool          textureLoaded;  ///< Whether the texture was successfully loaded

    // Audio
    float         masterVolume;   ///< Master volume [0.0 – 1.0]
    bool          muted;          ///< Whether audio is muted
    bool          draggingVolume; ///< True while the user drags the volume slider

    // Display
    bool          showFps;        ///< Whether the FPS counter is visible
} ParamsMenu_St;

// ────────────────────────────────────────────────----------------------------
// API
// ────────────────────────────────────────────────----------------------------

/** Initialize state and load the gear texture. Call once at startup. */
void paramsMenu_init(ParamsMenu_St* menu);

/** Handle mouse input (open/close, sliders, toggles, cycle buttons). */
void paramsMenu_update(ParamsMenu_St* menu);

/** Render the gear button and, if open, the settings panel. */
void paramsMenu_draw(ParamsMenu_St* menu);

/** Unload the gear texture. Call at shutdown. */
void paramsMenu_free(ParamsMenu_St* menu);


#endif // PARAMS_MENU_H