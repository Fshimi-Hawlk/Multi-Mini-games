/**
    @file editor/editor.c
    @author Fshimi-Hawlk
    @date 2026-03-28
    @brief Central editor initialization and high-level orchestration.
*/

#include "editor/editor.h"
#include "editor/types.h"
#include "editor/properties.h"
#include "widgets/scrollFrame.h"
#include "systemSettings.h"

// Editor state 
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

TerrainType_Et currentPaletteType = TERRAIN_NORMAL;

// Dynamic arrays (instantiated here)
SelectedIndices_St selectedIndices = {0};
Clipboard_St clipboard = {0};
bool clipboardHasData = false;
s32 pasteAnchorIndex = 4;   // default = center (index 4 in 3x3 grid)

TextButton_St btnLoad  = { .bounds = {300.0f, 8.0f, 70.0f, 34.0f}, .text = "Load",  .baseColor = BLUE };
TextButton_St btnSave  = { .bounds = {380.0f, 8.0f, 70.0f, 34.0f}, .text = "Save",  .baseColor = GREEN };
TextButton_St btnGenerate  = { .bounds = {460.0f, 8.0f,  70.0f, 34.0f}, .text = "Code",  .baseColor = PURPLE };

extern LobbyGame_St lobby_game;
extern void updateEditor(LobbyGame_St* game, float dt);
extern void drawEditor(const LobbyGame_St* const game);

static void editor_interface_init(void) {
    initEditor(&lobby_game);
}

static void editor_interface_update(float dt) {
    updateEditor(&lobby_game, dt);
}

static void editor_interface_draw(void) {
    drawEditor(&lobby_game);
}

GameClientInterface_St editorClientInterface = {
    .id      = MINI_GAME_EDITOR,
    .name    = "Level Editor",
    .init    = editor_interface_init,
    .update  = editor_interface_update,
    .draw    = editor_interface_draw,
    .on_data = NULL
};

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
    game->showLeftPalette = true;
    game->showPropertiesPanel = false;

    // Initialize scroll frames
    scrollFrameInit(&paletteScroll, (Rectangle){0, 80, 250, GetScreenHeight() - 80}, (Rectangle){0, 0, 250, 1000});
    scrollFrameInit(&helpScroll, (Rectangle){260, 80, 400, 500}, (Rectangle){0, 0, 400, 2000});
    scrollFrameInit(&propertiesScroll, (Rectangle){GetScreenWidth() - 300, 80, 300, GetScreenHeight() - 80}, (Rectangle){0, 0, 300, 2000});

    propertiesInit();

    // Camera backup for clean exit
    editorCameraBackup = game->cam;

    // Default palette selection
    currentPaletteType = TERRAIN_NORMAL;

    portalTargetPickMode = false;
    portalBeingConfigured = -1;
    highlightPortalIndex     = -1;
    highlightOtherPortalIndex = -1;
    highlightTimer           = 0.0f;

    editorAnyTextBoxActive = false;
    propertiesBeingEdited = false;

    log_info("Editor initialized");
}
