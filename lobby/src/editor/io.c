/**
    @file io.c
    @author Léandre BAUDET
    @date 2026-03-29
    @date 2026-04-13
    @brief Level editor file I/O with native dialogs and backup.
*/
#include "editor/io.h"
#include "editor/types.h"

#include "utils/globals.h"

#include "sharedUtils/mathUtils.h"

// Magic for our .dat files
#define LEVEL_FILE_MAGIC 0x4C4F4242u   // "LOBB"

static bool writeTerrainArray(FILE* f, const TerrainVec_St* da) {
    if (fwrite(&da->count, sizeof(size_t), 1, f) != 1) return false;
    if (da->count > 0) {
        if (fwrite(da->items, sizeof(LobbyTerrain_St), da->count, f) != da->count) return false;
    }
    return true;
}

static bool readTerrainArray(FILE* f, TerrainVec_St* da) {
    size_t count = 0;
    if (fread(&count, sizeof(size_t), 1, f) != 1) return false;

    da_clear(da);
    if (count == 0) return true;

    da_reserve(da, count);
    if (fread(da->items, sizeof(LobbyTerrain_St), count, f) != count) {
        da_clear(da);
        return false;
    }
    da->count = count;
    return true;
}
/**
    @brief Writes the game interaction zones array with proper string serialization
*/
static bool writeGIZArray(FILE* f) {
    u32 count = __miniGameIdCount;
    if (fwrite(&count, sizeof(u32), 1, f) != 1) return false;

    for (u32 i = 0; i < count; ++i) {
        const GameInteractionZone_St* z = &gameZones[i];

        // All fields are now POD → can be written directly
        if (fwrite(&z->hitbox, sizeof(Rectangle), 1, f) != 1) return false;
        if (fwrite(&z->color,  sizeof(Color),     1, f) != 1) return false;
        if (fwrite(&z->active, sizeof(bool),      1, f) != 1) return false;
        if (fwrite(z->name,    sizeof(char),     32, f) != 32)   return false;
    }
    return true;
}

/**
    @brief Reads the game interaction zones array.
*/
static bool readGIZArray(FILE* f) {
    u32 count = 0;
    if (fread(&count, sizeof(u32), 1, f) != 1) return false;

    memset(gameZones, 0, sizeof(gameZones));

    u32 toRead = min(count, (u32)__miniGameIdCount);
    for (u32 i = 0; i < toRead; ++i) {
        GameInteractionZone_St* z = &gameZones[i];

        if (fread(&z->hitbox, sizeof(Rectangle), 1, f) != 1) return false;
        if (fread(&z->color,  sizeof(Color),     1, f) != 1) return false;
        if (fread(&z->active, sizeof(bool),      1, f) != 1) return false;
        if (fread(z->name,    sizeof(char),     32, f) != 32)   return false;

        // Ensure null-termination (defensive)
        z->name[31] = '\0';
    }
    return true;
}

bool editorSaveLevel(const char* filename) {
    if (!filename) return false;

    FILE* f = fopen(filename, "wb");
    if (!f) {
        log_error("Failed to open for writing: %s", filename);
        return false;
    }

    u32 magic = LEVEL_FILE_MAGIC;
    u32 version = 1;
    fwrite(&magic, sizeof(u32), 1, f);
    fwrite(&version, sizeof(u32), 1, f);

    bool ok = writeTerrainArray(f, &terrains);
    if (ok) ok = writeGIZArray(f);
    fclose(f);

    if (ok) {
        log_info("Level saved: %s (%zu terrains)", filename, terrains.count);
    } else {
        log_error("Write failed for %s", filename);
    }

    return ok;
}

bool editorLoadLevel(const char* filename) {
    if (!filename) return false;

    FILE* f = fopen(filename, "rb");
    if (!f) {
        log_warn("Level not found: %s", filename);
        return false;
    }

    u32 magic = 0, version = 0;
    bool ok = (fread(&magic, sizeof(u32), 1, f) == 1 &&
               fread(&version, sizeof(u32), 1, f) == 1 &&
               magic == LEVEL_FILE_MAGIC && version == 1);

    if (ok) ok = readTerrainArray(f, &terrains);
    if (ok) ok = readGIZArray(f);

    fclose(f);

    if (ok) {
        log_info("Level loaded: %s (%zu terrains)", filename, terrains.count);
        da_clear(&selectedIndices);
    } else {
        log_error("Invalid/corrupted level file: %s", filename);
    }
    return ok;
}

bool editorCreateBackup(const char* baseName) {
    if (!baseName || !baseName[0]) return false;

    char backup[512];
    time_t now = time(NULL);
    struct tm* tm = localtime(&now);

    snprintf(backup, sizeof(backup),
             "assets/levels/backups/%s_%04d%02d%02d_%02d%02d%02d.dat",
             baseName, tm->tm_year+1900, tm->tm_mon+1, tm->tm_mday,
             tm->tm_hour, tm->tm_min, tm->tm_sec);

    // Simple copy
    FILE* src = fopen("assets/levels/defaultLevel.dat", "rb"); // fallback to default if needed
    // Better: use the actual current filename, but for simplicity we backup the default slot
    // You can improve later by passing current filename.

    if (!src) return false;
    FILE* dst = fopen(backup, "wb");
    if (!dst) { fclose(src); return false; }

    char buf[4096];
    size_t n;
    while ((n = fread(buf, 1, sizeof(buf), src)) > 0) fwrite(buf, 1, n, dst);

    fclose(src);
    fclose(dst);
    log_info("Backup created: %s", backup);
    return true;
}

char* editorShowOpenDialog(const char* defaultPath) {
    char cmd[1024];
    snprintf(cmd, sizeof(cmd),
        "zenity --file-selection --title=\"Load Level\" --file-filter=\"Level files (*.dat)|*.dat\" %s 2>/dev/null",
        defaultPath ? "--filename=\"$HOME/assets/levels/\"" : "");

    FILE* pipe = popen(cmd, "r");
    if (!pipe) return NULL;

    char path[1024] = {0};
    if (fgets(path, sizeof(path), pipe) == NULL) {
        pclose(pipe);
        return NULL;
    }
    pclose(pipe);

    // trim newline
    size_t len = strlen(path);
    if (len > 0 && path[len-1] == '\n') path[len-1] = '\0';

    if (path[0] == '\0') return NULL;

    return strdup(path);   // caller must free()
}

char* editorShowSaveDialog(const char* defaultName, const char* ext) {
    char cmd[1024];
    snprintf(cmd, sizeof(cmd),
        "zenity --file-selection --save --confirm-overwrite --title=\"Save Level\" "
        "--file-filter=\"Level files (*%s)|*%s\" --filename=\"" ASSET_PATH "levels/%s%s\" 2>/dev/null",
        ext, ext, defaultName ? defaultName : "untitledLevel", ext
    );

    FILE* pipe = popen(cmd, "r");
    if (!pipe) return NULL;

    char path[1024] = {0};
    if (fgets(path, sizeof(path), pipe) == NULL) {
        pclose(pipe);
        return NULL;
    }
    pclose(pipe);

    size_t len = strlen(path);
    if (len > 0 && path[len-1] == '\n') path[len-1] = '\0';

    if (path[0] == '\0') return NULL;

    return strdup(path);
}