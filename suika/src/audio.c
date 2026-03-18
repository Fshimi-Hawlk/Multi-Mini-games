#include "audio.h"
#include "raylib.h"

Sound sound_drop;
Sound sound_merge;

static const char* getSoundPath(const char* filename) {
    // Try standard path first (when run from suika directory)
    static char standardPath[256];
    snprintf(standardPath, sizeof(standardPath), "assets/sounds/%s", filename);
    
    if (FileExists(standardPath)) {
        return standardPath;
    }
    
    // Try relative to suika directory (when run from lobby)
    static char suikaPath[256];
    snprintf(suikaPath, sizeof(suikaPath), "../suika/assets/sounds/%s", filename);
    
    if (FileExists(suikaPath)) {
        return suikaPath;
    }
    
    // Fallback to standard path (will likely fail but lets raylib handle it)
    return standardPath;
}

void initAudio(void) {
    sound_drop = LoadSound(getSoundPath("suika_drop.wav"));
    sound_merge = LoadSound(getSoundPath("suika_merge.wav"));
}

void freeAudio(void) {
    UnloadSound(sound_drop);
    UnloadSound(sound_merge);
}