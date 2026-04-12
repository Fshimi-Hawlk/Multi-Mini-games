/**
    @file editor/types.h
    @author Fshimi-Hawlk
    @date 2026-03-27
    @date 2026-03-27
    @brief Private types and enums used internally by the level editor.

    This header should only be included by files inside the editor module.
*/

#ifndef EDITOR_TYPES_H
#define EDITOR_TYPES_H

#include "sharedWidgets/types.h"

#include "utils/userTypes.h"

// Dynamic array of selected terrain indices
typeDA(s32, SelectedIndices_St);

// Clipboard for copy/paste
typeDA(LobbyTerrain_St, Clipboard_St);

typedef enum {
    DRAG_NONE,
    DRAG_PLACING_NEW,
    DRAG_MOVING,
    DRAG_RESIZING,
    DRAG_MULTI_SELECT
} EditorDragMode_Et;

typedef enum {
    HANDLE_NONE = 0,
    HANDLE_TOP_LEFT,
    HANDLE_TOP,
    HANDLE_TOP_RIGHT,
    HANDLE_RIGHT,
    HANDLE_BOTTOM_RIGHT,
    HANDLE_BOTTOM,
    HANDLE_BOTTOM_LEFT,
    HANDLE_LEFT
} ResizeHandle_Et;

extern EditorDragMode_Et editorDragMode;
extern ResizeHandle_Et activeHandle;
extern Vector2 dragStartWorld;
extern Rectangle dragPreviewRect;
extern Rectangle multiSelectRect;
extern f32 panMultiplier;

extern bool showHelpPanel;
extern bool showSelectionBox;
extern bool duplicationOffsetEnabled;
extern bool immediateSelectOnClick;      ///< When true: single click on another terrain replaces selection immediately
extern bool immediateDragAfterSelect;    ///< When true: first click on a terrain selects + starts dragging it

extern ScrollFrame_St paletteScroll;
extern ScrollFrame_St helpScroll;
extern ScrollFrame_St propertiesScroll;
extern bool propertiesBeingEdited;
extern bool editorAnyTextBoxActive;

extern bool portalTargetPickMode;        ///< True when waiting for next click to set portal target
extern s32  portalBeingConfigured;       ///< Index of the portal currently being configured (-1 = none)
extern s32  highlightPortalIndex;        ///< Index of portal that should show highlight (-1 = none)
extern s32  highlightOtherPortalIndex;   ///< For two-way: the linked portal
extern f32  highlightTimer;              ///< Seconds left for the highlight

extern Camera2D editorCameraBackup;

extern TerrainKind_Et currentPaletteKind;

extern SelectedIndices_St selectedIndices;
extern Clipboard_St clipboard;
extern bool clipboardHasData;
extern s32 pasteAnchorIndex;

extern TextButton_St btnLoad;
extern TextButton_St btnSave;
extern TextButton_St btnGenerate;

#endif // EDITOR_TYPES_H