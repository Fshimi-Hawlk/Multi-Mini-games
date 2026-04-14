/**
    @file editor/types.h
    @author Fshimi-Hawlk
    @date 2026-03-27
    @date 2026-04-14
    @brief Private types and enums used internally by the level editor.
*/
#ifndef EDITOR_TYPES_H
#define EDITOR_TYPES_H

#include "utils/userTypes.h"
#include "sharedWidgets/types.h"

// Dynamic array of selected terrain indices
typeDA(s32, SelectedIndices_St);

// Clipboard for copy/paste
typeDA(LobbyTerrain_St, Clipboard_St);

/**
    @brief Definition of typedef enum
*/
typedef enum {
    DRAG_NONE,
    DRAG_PLACING_NEW,
    DRAG_MOVING,
    DRAG_RESIZING,
    DRAG_MULTI_SELECT,
    DRAG_MOVING_ZONE
} EditorDragMode_Et;

/**
    @brief Definition of typedef enum
*/
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

/**
    @brief Global variable editorDragMode
*/
static const char* terrainKindNames[__terrainKindCount] = {
    "Normal",
    "Grass",
    "Wood Plank",
    "Stone",
    "Ice",
    "Bouncy",
    "Moving H",
    "Moving V",
    "Water",
    "Decorative",
    "Portal"
};
extern EditorDragMode_Et editorDragMode;
/**
    @brief Global variable activeHandle
*/
extern ResizeHandle_Et activeHandle;
/**
    @brief Global variable dragStartWorld
*/
extern Vector2 dragStartWorld;
/**
    @brief Global variable dragPreviewRect
*/
extern Rectangle dragPreviewRect;
/**
    @brief Global variable multiSelectRect
*/
extern Rectangle multiSelectRect;
/**
    @brief Global variable panMultiplier
*/
extern f32 panMultiplier;

/**
    @brief Global variable showHelpPanel
*/
extern bool showHelpPanel;
/**
    @brief Global variable showSelectionBox
*/
extern bool showSelectionBox;
/**
    @brief Global variable duplicationOffsetEnabled
*/
extern bool duplicationOffsetEnabled;
extern bool immediateSelectOnClick;      ///< When true: single click on another terrain replaces selection immediately
extern bool immediateDragAfterSelect;    ///< When true: first click on a terrain selects + starts dragging it

/**
    @brief Global variable paletteScroll
*/
extern ScrollFrame_St paletteScroll;
/**
    @brief Global variable helpScroll
*/
extern ScrollFrame_St helpScroll;
/**
    @brief Global variable propertiesScroll
*/
extern ScrollFrame_St propertiesScroll;
/**
    @brief Global variable propertiesBeingEdited
*/
extern bool propertiesBeingEdited;
/**
    @brief Global variable editorAnyTextBoxActive
*/
extern bool editorAnyTextBoxActive;

extern bool portalTargetPickMode;        ///< True when waiting for next click to set portal target
extern s32  portalBeingConfigured;       ///< Index of the portal currently being configured (-1 = none)
extern s32  highlightPortalIndex;        ///< Index of portal that should show highlight (-1 = none)
extern s32  highlightOtherPortalIndex;   ///< For two-way: the linked portal
extern f32  highlightTimer;              ///< Seconds left for the highlight

/**
    @brief Global variable editorCameraBackup
*/
extern Camera2D editorCameraBackup;

/**
    @brief Global variable currentPaletteKind
*/
extern TerrainKind_Et currentPaletteKind;

/**
    @brief Global variable selectedIndices
*/
extern SelectedIndices_St selectedIndices;
/**
    @brief Global variable clipboard
*/
extern Clipboard_St clipboard;
/**
    @brief Global variable clipboardHasData
*/
extern bool clipboardHasData;
/**
    @brief Global variable pasteAnchorIndex
*/
extern s32 pasteAnchorIndex;

/**
    @brief Global variable btnLoad
*/
extern TextButton_St btnLoad;
/**
    @brief Global variable btnSave
*/
extern TextButton_St btnSave;
/**
    @brief Global variable btnGenerate
*/
extern TextButton_St btnGenerate;

/**
    @brief Index of the currently selected game interaction zone (-1 = none).
           Used for dragging the mini-game entry zones.
*/
extern s32 selectedZoneIndex;

#endif // EDITOR_TYPES_H