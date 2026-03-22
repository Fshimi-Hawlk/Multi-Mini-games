/**
 * @file progress.c
 * @author Gemini CLI
 * @date 2026-03-22
 * @brief Implementation of player progress saving/loading.
 */

#include "APIs/progressAPI.h"
#include <stdio.h>
#include <string.h>

#define PROGRESS_FILE "progress.dat"

void SaveProgress(const PlayerProgress_St* progress) {
    FILE* f = fopen(PROGRESS_FILE, "wb");
    if (f) {
        fwrite(progress, sizeof(PlayerProgress_St), 1, f);
        fclose(f);
    }
}

PlayerProgress_St LoadProgress(void) {
    PlayerProgress_St progress;
    memset(&progress, 0, sizeof(PlayerProgress_St));
    
    FILE* f = fopen(PROGRESS_FILE, "rb");
    if (f) {
        fread(&progress, sizeof(PlayerProgress_St), 1, f);
        fclose(f);
    } else {
        // Initial defaults
        progress.has_crown = false;
        for (s32 g = 0; g < MAX_GAMES_PROGRESS; g++) {
            progress.current_ap[g] = AP_COMMON;
        }
    }
    return progress;
}
