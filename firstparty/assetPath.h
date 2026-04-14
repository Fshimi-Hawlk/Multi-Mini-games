/**
    @file assetPath.h
    @author Multi Mini-Games Team
    @date 2026-04-14
    @date 2026-04-14
    @brief Utility to locate asset files from multiple candidate paths.
*/
#ifndef FIRSTPARTY_ASSET_PATH_H
#define FIRSTPARTY_ASSET_PATH_H

#include <stdio.h>
#include <string.h>

/** @brief Check if a file exists using standard C (no raylib needed). */
static inline int assetFileExists(const char* path) {
    FILE* f = fopen(path, "rb");
    if (f) { fclose(f); return 1; }
    return 0;
}

/**
    @brief Find the directory prefix that contains a given filename.
    Tries each candidate prefix until prefix+filename is accessible.
    Writes the winning prefix into outBase, or "" if none found.
*/
static inline void findAssetBase(
    const char*  filename,
    const char** candidates,
    char*        outBase,
    int          outSize)
{
    char testPath[512];
    for (int i = 0; candidates[i] != NULL; i++) {
        snprintf(testPath, sizeof(testPath), "%s%s", candidates[i], filename);
        if (assetFileExists(testPath)) {
            snprintf(outBase, outSize, "%s", candidates[i]);
            return;
        }
    }
    outBase[0] = '\0';
}

/**
    @brief Resolve a relative asset path by trying multiple candidate prefixes.
    Writes the first existing path into outPath, or the bare relPath on failure.
    Returns 1 if found, 0 otherwise.
*/
static inline int findAssetPath(
    const char*  relPath,
    char*        outPath,
    int          outSize,
    const char** candidates)
{
    for (int i = 0; candidates[i] != NULL; i++) {
        snprintf(outPath, outSize, "%s%s", candidates[i], relPath);
        if (assetFileExists(outPath)) return 1;
    }
    snprintf(outPath, outSize, "%s", relPath);
    return 0;
}

#endif // FIRSTPARTY_ASSET_PATH_H
