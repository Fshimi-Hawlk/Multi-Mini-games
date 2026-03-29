/**
    @file editor/io.h
    @author Fshimi-Hawlk
    @date 2026-03-29
    @brief Level editor save / load / backup + native file dialogs.
*/

#ifndef EDITOR_IO_H
#define EDITOR_IO_H

#include "utils/userTypes.h"

/**
    @brief Saves current terrains to a .dat file (binary).
    @param filename Full path (e.g. "assets/levels/myLevel.dat")
    @return true on success
*/
bool editorSaveLevel(const char* filename);

/**
    @brief Loads terrains from a .dat file (replaces current level).
    @param filename Full path
    @return true on success
*/
bool editorLoadLevel(const char* filename);

/**
    @brief Creates a timestamped backup before overwriting.
    @param baseName Base name without extension (e.g. "defaultLevel")
    @return true if backup created
*/
bool editorCreateBackup(const char* baseName);

/**
    @brief Shows native file open dialog (via zenity) and returns selected path.
    @param defaultPath Optional starting directory or filename
    @return Allocated path string (must be freed by caller) or NULL if cancelled
*/
char* editorShowOpenDialog(const char* defaultPath);

/**
    @brief Shows native file save dialog (via zenity).
    @param defaultName Suggested filename (e.g. "myLevel")
    @return Allocated path string or NULL if cancelled
*/
char* editorShowSaveDialog(const char* defaultName);

#endif // EDITOR_IO_H