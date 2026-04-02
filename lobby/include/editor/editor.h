/**
    @file editor/editor.h
    @author Fshimi-Hawlk
    @date 2026-03-27
    @date 2026-03-27
    @brief Public interface for the lobby level editor.

    Provides high-level functions to toggle, update and draw the editor.
    All editor-specific state and logic is kept inside the editor module.

    @see editor/types.h
    @see editor/input.c
    @see editor/draw.c
*/

#ifndef EDITOR_EDITOR_H
#define EDITOR_EDITOR_H

#include "utils/userTypes.h"

/**
    @brief Initializes all editor subsystems and state.
           Should be called once when entering editor mode (in toggleEditorMode).
    @param game Pointer to the lobby game state
*/
void initEditor(LobbyGame_St* const game);

/**
    @brief Toggles editor mode on/off with F1 key.
    When entering editor mode the game is paused.
    When exiting, current editor changes stay in memory.
*/
void toggleEditorMode(LobbyGame_St* const game);

/**
    @brief Performs editor logic (input, drag, keyboard, etc.)
    @param game  Lobby game state
    @param dt    Delta time (reserved for future use)
*/
void updateEditor(LobbyGame_St* const game, f32 dt);

/**
    @brief Synchronizes the property edit buffers with the currently selected terrain(s).
           Called whenever selection changes or a value is committed.
    @param game Pointer to the lobby game state
*/
void refreshPropertyBuffers(const LobbyGame_St* const game);

/**
    @brief Draws the editor overlay (both world-space and screen-space UI).
    Only draws when editorMode is active.
    @param game  Const pointer to lobby game state
*/
void drawEditor(const LobbyGame_St* const game);

#endif // EDITOR_EDITOR_H