/**
    @file editor/editor.c
    @author Fshimi-Hawlk
    @date 2026-03-28
    @date 2026-04-14
    @brief Central editor initialization and high-level orchestration.
*/

#include "editor/editor.h"
#include "editor/types.h"
#include "editor/properties.h"

#include "sharedWidgets/scrollFrame.h"

// ── Editor initialization ───────────────────────────────────────────────────
EditorDragMode_Et editorDragMode = DRAG_NONE;
ResizeHandle_Et activeHandle = HANDLE_NONE;
Vector2 dragStartWorld = {0.0f, 0.0f};
Rectangle dragPreviewRect = {0};
Rectangle multiSelectRect = {0};
f32 panMultiplier = 1.0f;

bool duplicationOffsetEnabled = true;
bool immediateSelectOnClick     = true;
bool immediateDragAfterSelect   = false;

bool showHelpPanel = false;
bool showSelectionBox = false;

bool portalTargetPickMode = false;
s32 portalBeingConfigured = -1;
s32 highlightPortalIndex = -1;
s32 highlightOtherPortalIndex = -1;
f32 highlightTimer = 0.0f;

bool editorAnyTextBoxActive = false;
bool propertiesBeingEdited = false;

ScrollFrame_St paletteScroll    = {0};
ScrollFrame_St helpScroll       = {0};
ScrollFrame_St propertiesScroll = {0};

Camera2D editorCameraBackup = {0};

TerrainKind_Et currentPaletteKind = TERRAIN_KIND_NORMAL;

// Dynamic arrays (instantiated here)
SelectedIndices_St selectedIndices = {0};
Clipboard_St clipboard = {0};
bool clipboardHasData = false;
s32 pasteAnchorIndex = 4;   // default = center (index 4 in 3x3 grid)

TextButton_St btnLoad  = { .bounds = {300.0f, 8.0f, 70.0f, 34.0f}, .text = "Load", .textColor = WHITE, .baseColor = BLUE };
TextButton_St btnSave  = { .bounds = {380.0f, 8.0f, 70.0f, 34.0f}, .text = "Save", .textColor = WHITE, .baseColor = GREEN };
TextButton_St btnGenerate  = { .bounds = {460.0f, 8.0f,  70.0f, 34.0f}, .text = "Code", .textColor = WHITE, .baseColor = PURPLE };

s32 selectedZoneIndex = -1;

void initEditor(LobbyGame_St* const game) {
    // Reset drag & selection state
    editorDragMode = DRAG_NONE;
    activeHandle = HANDLE_NONE;

    showHelpPanel = false;
    showSelectionBox = true;

    da_clear(&selectedIndices);
    da_clear(&clipboard);
    clipboardHasData = false;
    pasteAnchorIndex = 4;                     // center anchor

    game->selectedTerrainIndex = -1;
    selectedZoneIndex = -1;

    game->showLeftPalette = true;
    game->showPropertiesPanel = false;

    // Initialize scroll frames
    scrollFrameInit(
        &paletteScroll, 
        (Rectangle) {0, 80, 250, systemSettings.video.height - 80},
        (f32Vector2) {250, 1000}, 
        60.0f, 0.1f
    );

    scrollFrameInit(
        &helpScroll, 
        (Rectangle) {260, 80, 400, 500},
        (f32Vector2) {400, 2000}, 
        60.0f, 0.1f
    );

    scrollFrameInit(
        &propertiesScroll, 
        (Rectangle) {systemSettings.video.width - 300, 80, 300, systemSettings.video.height - 80},
        (f32Vector2) {300, 2000}, 
        60.0f, 0.1f
    );

    propertiesInit();

    // Camera backup for clean exit
    editorCameraBackup = game->cam;

    // Start with an empty centered canvas
    game->cam.target = (f32Vector2) {0.0f, 0.0f};
    game->cam.offset = (f32Vector2) {systemSettings.video.width / 2.0f, systemSettings.video.height / 2.0f};
    game->cam.zoom = 0.5f;

    // Default palette selection
    currentPaletteKind = TERRAIN_KIND_NORMAL;

    portalTargetPickMode = false;
    portalBeingConfigured = -1;
    highlightPortalIndex     = -1;
    highlightOtherPortalIndex = -1;
    highlightTimer           = 0.0f;

    editorAnyTextBoxActive = false;
    propertiesBeingEdited = false;

    log_info("Editor initialized");
}