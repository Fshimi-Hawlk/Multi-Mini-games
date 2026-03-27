/**
    @file ui/editor.h
    @author Grok (assisted)
    @date 2026-03-27
    @brief Public interface for the lobby level editor overlay.

    Provides simple toggle and separate update/draw functions for editor mode.
    All drawing is screen-space UI only. World rendering stays in lobby_draw().

    @see ui/editor.c
*/

#ifndef UI_EDITOR_H
#define UI_EDITOR_H

#include "utils/userTypes.h"

/**
    @brief Toggles editor mode on/off with F1 key.
    When entering editor mode the game is paused.
    When exiting, current editor changes stay in memory (apply later).
*/
void toggleEditorMode(LobbyGame_St* const game);

/**
    @brief Performs editor logic
    @param game  Lobby game state
    @param dt    Delta time (reserved for future editor tools)
*/
void updateEditor(LobbyGame_St* const game, f32 dt);

/**
    @brief Draws the editor HUD overlay (screen-space).
    Only draws when editorMode is active.
    @param game  Const pointer to lobby game state
*/
void drawEditor(const LobbyGame_St* const game);

#endif // UI_EDITOR_H