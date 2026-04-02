/**
    @file editor/codegen.h
    @author Fshimi-Hawlk
    @date 2026-03-30
    @brief Generates clean C source code from current editor terrain data.
*/

#ifndef EDITOR_CODEGEN_H
#define EDITOR_CODEGEN_H

#include "utils/userTypes.h"

/**
    @brief Generates a C source snippet for the current `terrains` array and writes it to a file.
           The output matches the exact style used in setups/game.c with grouped comments.

    @param game      Pointer to LobbyGame_St (for logging only)
    @param filename  Output .c file (e.g. "assets/levels/generatedLevel.c")
    @return true on success
*/
bool editorGenerateCode(const LobbyGame_St* const game, const char* filename);

#endif // EDITOR_CODEGEN_H