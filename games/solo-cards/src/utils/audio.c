/**
    @file audio.c
    @author Maxime CHAUVEAU
    @date February 2026
    @date 2026-04-14
    @brief Solitaire audio functions
*/
#include "utils/audio.h"
#include "assetPath.h"
#include <stdio.h>

Sound sound_cardTurn;

static char g_solitaireSoundsPath[512] = {0};

static const char* solitaire_getSoundsPath(void) {
    if (g_solitaireSoundsPath[0] != '\0') return g_solitaireSoundsPath;
    static const char* candidates[] = {
        SOUNDS_PATH,                    /* "jeux/solitaire/assets/sounds/" from project root */
        "jeux/solitaire/assets/sounds/",
        "assets/sounds/",
        NULL
    };
    findAssetBase("solitaire_cardTurn.wav", candidates,
                  g_solitaireSoundsPath, sizeof(g_solitaireSoundsPath));
    return g_solitaireSoundsPath;
}

void solitaire_initAudio(void) {
    char p[600];
    snprintf(p, sizeof(p), "%ssolitaire_cardTurn.wav", solitaire_getSoundsPath());
    sound_cardTurn = LoadSound(p);
}

void solitaire_freeAudio(void) {
    UnloadSound(sound_cardTurn);
}
