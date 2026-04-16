/**
    @file prompt.c
    @author Fshimi-Hawlk
    @date 2026-04-07
    @date 2026-04-14
    @brief Save/load modal UI implementation using widgets.
*/
#include "core/game.h"

#include "ui/prompt.h"
#include "ui/game.h"

#include "setups/save.h"
#include "setups/game.h"

#include "utils/globals.h"

#include "sharedWidgets/button.h"
#include "sharedWidgets/textBox.h"
#include "sharedWidgets/scrollFrame.h"

#include <sys/stat.h>
#include <dirent.h>

/**
    @brief One save-file entry for the UI list (lightweight metadata only).
*/
typedef struct {
    TextButton_St button;       ///< Widget base
    ImageButton_St delButton;   ///< Delete button
    char filename[64];          ///< Base name only (e.g. "mygame.sav")
    u64  score;                 ///< Current score
    u8   streakCount;           ///< Current streak
    bool hasBeenLost;           ///< True if this save has already reached gameOver once
    time_t timestamp;           ///< File modification time (for sorting)
} SaveEntry_St;

typeDA(SaveEntry_St, SaveEntryVec_St);

/* Private static widgets — bounds recalculated at runtime via promptRepositionButtons() */
static TextButton_St btnNewGame     = { .text = "New Game",    .textColor = WHITE, .baseColor = GREEN,  .roundness = 0.2f };
static TextButton_St btnLoadSave    = { .text = "Load Save",   .textColor = WHITE, .baseColor = BLUE,   .roundness = 0.2f };

static TextButton_St btnYesSave     = { .text = "Yes",         .textColor = WHITE, .baseColor = GREEN,  .roundness = 0.2f };
static TextButton_St btnNoSave      = { .text = "No",          .textColor = WHITE, .baseColor = RED,    .roundness = 0.2f };

static TextButton_St btnSaveFile    = { .text = "Save",        .textColor = WHITE, .baseColor = GREEN,  .roundness = 0.2f };
static TextButton_St btnCancelFile  = { .text = "Cancel",      .textColor = WHITE, .baseColor = RED,    .roundness = 0.2f };

static TextButton_St btnCancelLoad  = { .text = "Cancel",      .textColor = WHITE, .baseColor = RED,    .roundness = 0.2f };
static TextButton_St btnLoadChosen  = { .text = "Load",        .textColor = WHITE, .baseColor = BLUE,   .roundness = 0.2f };

static TextButton_St btnYesDelete   = { .text = "Yes, Delete", .textColor = WHITE, .baseColor = RED,    .roundness = 0.25f };
static TextButton_St btnNoDelete    = { .text = "No, Cancel",  .textColor = WHITE, .baseColor = GRAY,   .roundness = 0.25f };

static TextBox_St filenameBox       = { .roundness = 0.2f, .placeholder = "my_save_name" };

/** Repositions all prompt buttons relative to the current screen size. */
static void promptRepositionButtons(void) {
    f32 sw = (f32)GetScreenWidth();
    f32 sh = (f32)GetScreenHeight();
    f32 cx = sw / 2.0f;
    f32 cy = sh / 2.0f;

    btnNewGame.bounds    = (Rectangle){cx - 210.0f, cy,         200, 50};
    btnLoadSave.bounds   = (Rectangle){cx +  10.0f, cy,         200, 50};

    btnYesSave.bounds    = (Rectangle){cx - 160.0f, cy,         150, 50};
    btnNoSave.bounds     = (Rectangle){cx +  10.0f, cy,         150, 50};

    btnSaveFile.bounds   = (Rectangle){cx - 160.0f, cy + 120.0f, 150, 50};
    btnCancelFile.bounds = (Rectangle){cx +  10.0f, cy + 120.0f, 150, 50};
    filenameBox.bounds   = (Rectangle){cx - 200.0f, cy +  55.0f, 400, 50};

    btnCancelLoad.bounds = (Rectangle){cx - 160.0f, sh - 80.0f,  150, 50};
    btnLoadChosen.bounds = (Rectangle){cx +  10.0f, sh - 80.0f,  150, 50};

    btnYesDelete.bounds  = (Rectangle){cx - 150.0f, cy +  40.0f, 140, 55};
    btnNoDelete.bounds   = (Rectangle){cx +  10.0f, cy +  40.0f, 140, 55};
}

static ScrollFrame_St saveListScroll = {0};
static SaveEntryVec_St saveList = {0};
static s32 selectedSaveIndex = -1;

static s32 pendingDeleteIndex = -1;

static const char* getSaveDirectory(void) {
    static bool created = false;
    if (!created) {
        mkdir(SAVES_PATH, 0755);
        created = true;
    }
    return SAVES_PATH;
}

static void listSaves(SaveEntryVec_St* outList) {
    da_clear(outList);
    DIR* dir = opendir(getSaveDirectory());
    if (!dir) return;

    struct dirent* entry;
    while ((entry = readdir(dir))) {
        if (strstr(entry->d_name, ".sav") == NULL) continue;

        char fullPath[512];
        size_t fullPathLen = strlen(SAVES_PATH) + strlen(entry->d_name);
        if (fullPathLen >= sizeof(fullPath)) continue;
        snprintf(fullPath, sizeof(fullPath), "%s%s", SAVES_PATH, entry->d_name);

        struct stat st;
        if (stat(fullPath, &st) != 0) continue;

        // Quick header read for metadata (score, streak, hasBeenLost)
        FILE* f = fopen(fullPath, "rb");
        if (!f) continue;

        u8 header[64] = {0};
        if (fread(header, 1, 64, f) != 64) {
            fclose(f);
            continue;
        }
        fclose(f);

        SaveEntry_St entryData = {0};
        strncpy(entryData.filename, entry->d_name, 63);

        memcpy(&entryData.score, header + 6, sizeof(u64));
        memcpy(&entryData.streakCount, header + 14, sizeof(u8));

        fseek(f, -1, SEEK_END);
        u8 lostFlag = 0;
        if (fread(&lostFlag, 1, 1, f) == 1) {
            entryData.hasBeenLost = (lostFlag != 0);
        }
        entryData.timestamp = st.st_mtime;

        da_append(outList, entryData);
    }
    closedir(dir);

    // Sort newest first
    // (simple bubble for small list - fine)
    for (u32 i = 0; i < outList->count; ++i) {
        for (u32 j = i + 1; j < outList->count; ++j) {
            if (outList->items[i].timestamp < outList->items[j].timestamp) {
                SaveEntry_St tmp = outList->items[i];
                outList->items[i] = outList->items[j];
                outList->items[j] = tmp;
            }
        }
    }
}

static void freeSaveList(SaveEntryVec_St* list) {
    da_clear(list);
}

static bool deleteSave(const char* filename) {
    char path[256];
    snprintf(path, sizeof(path), "%s%s", SAVES_PATH, filename);
    return (remove(path) == 0);
}

bool polyBlast_loadGameFromFile(PolyBlastGame_St* const state, const char* filename) {
    if (!state || !filename) return false;

    char path[256];
    snprintf(path, sizeof(path), "%s%s", SAVES_PATH, filename);
    if (strstr(filename, ".sav") == NULL) {
        snprintf(path, sizeof(path), "%s%s.sav", SAVES_PATH, filename);
    }

    FILE* f = fopen(path, "rb");
    if (!f) return false;

    fseek(f, 0, SEEK_END);
    u64 size = ftell(f);
    fseek(f, 0, SEEK_SET);

    u8* buffer = calloc(size, sizeof(*buffer));
    if (buffer == NULL) {
        log_fatal("Couldn't allocate buffer");

        fclose(f);
        return false;
    }

    if (fread(buffer, 1, size, f) != size) {
        free(buffer);
        fclose(f);
        return false;
    }
    fclose(f);

    bool ok = polyBlast_deserializeGameState(state, buffer, size, true);
    free(buffer);

    state->loadFilename = context_strdup(filename);

    if (ok && state->hasBeenLost) {
        log_info("Loaded previously-lost save (stats will not be recorded for leaderboards)");
    }

    polyBlast_buildScoreRelatedTexts(&state->scoring);

    return ok;
}

static void promptInitLoadList(void) {
    selectedSaveIndex = -1;
    listSaves(&saveList);

    log_debug("retrieved save count: %d", saveList.count);

    const f32 buttonHeight = 32;
    const f32 buttonPadding = 10;
    const f32 increment = (buttonPadding * 2 + buttonHeight);

    scrollFrameInit(
        &saveListScroll, (Rectangle) {100, 200, 800, 300},
        (Vector2) {0, saveList.count * increment},
        60.0f, 0.025f
    );

    f32 y = saveListScroll.scroll.y + saveListScroll.visibleArea.y + 10;

    for (u32 i = 0; i < saveList.count; ++i) {
        SaveEntry_St* entry = &saveList.items[i];
        TextButton_St* button = &entry->button;

        button->state = WIDGET_STATE_NORMAL;
        button->textColor = WHITE;
        button->text = TextFormat(
            "%-24s %-3s%-8lu%-3s %-3s%-3u%-3s %s",
            entry->filename,
            "", entry->score, "",
            "", entry->streakCount, "",
            entry->hasBeenLost ? "[LOST]" : "[VALID]"
        );

        button->bounds = (Rectangle) {
            saveListScroll.visibleArea.x + buttonPadding, y,
            saveListScroll.visibleArea.width - buttonPadding * 2 - buttonHeight - 5 * 2,
            buttonHeight
        };
        button->roundness = 0.2f;

        ImageButton_St* delButton = &entry->delButton;
        delButton->bounds = (Rectangle) {
            .x = button->bounds.x + button->bounds.width + 10,
            .y = y,
            .width = buttonHeight,
            .height = buttonHeight
        };

        delButton->roundness = 0.1f;

        y += increment;
    }

}

bool polyBlast_promptUpdate(PolyBlastGame_St* const game, Vector2 mouseScreen) {
    if (polyBlast_currentPrompt == PROMPT_NONE) return false;

    promptRepositionButtons();

    bool shouldCloseWindow = false;

    switch (polyBlast_currentPrompt) {
        case PROMPT_START_LOAD: {
            if (textButtonUpdate(&btnNewGame, mouseScreen)) {
                polyBlast_resetGame(game);
                polyBlast_currentPrompt = PROMPT_NONE;
            }

            if (textButtonUpdate(&btnLoadSave, mouseScreen)) {
                promptInitLoadList();
                polyBlast_currentPrompt = PROMPT_SAVES_LIST;
            }
        } break;

        case PROMPT_SAVE_QUIT: {
            if (textButtonUpdate(&btnYesSave, mouseScreen)) {
                polyBlast_currentPrompt = PROMPT_SAVE_FILENAME;
                textBoxUpdate(&filenameBox, mouseScreen); // init box
            }

            if (textButtonUpdate(&btnNoSave, mouseScreen) || IsKeyPressed(KEY_ESCAPE)) {
                shouldCloseWindow = true;
            }
        } break;

        case PROMPT_SAVE_FILENAME: {
            textBoxUpdate(&filenameBox, mouseScreen);

            const char *filename = filenameBox.buffer;
            bool filenameEmpty = filename[0] == '\0';
            if (textButtonUpdate(&btnSaveFile, mouseScreen) && !filenameEmpty) {
                if (polyBlast_saveGameToFile(game, filename)) {
                    log_info("Game saved as \"%s\"", filename);
                }

                polyBlast_currentPrompt = PROMPT_NONE;
                shouldCloseWindow = true;
            }

            if (textButtonUpdate(&btnCancelFile, mouseScreen) || IsKeyPressed(KEY_ESCAPE)) {
                shouldCloseWindow = true;
            }
        } break;

        case PROMPT_SAVES_LIST: {
            scrollFrameUpdate(&saveListScroll, saveListScroll.visibleArea, mouseScreen);

            if (textButtonUpdate(&btnCancelLoad, mouseScreen)) {
                polyBlast_resetGame(game);
                freeSaveList(&saveList);
                polyBlast_currentPrompt = PROMPT_NONE;
            }

            bool canLoad = (saveList.count > 0 && selectedSaveIndex != -1);
            if (canLoad && textButtonUpdate(&btnLoadChosen, mouseScreen)) {
                if (polyBlast_loadGameFromFile(game, saveList.items[selectedSaveIndex].filename)) {
                    log_info("Loaded save: %s", saveList.items[selectedSaveIndex].filename);
                }
                freeSaveList(&saveList);
                polyBlast_currentPrompt = PROMPT_NONE;
            }

            bool anySaveSelected = false;
            for (u32 i = 0; i < saveList.count; ++i) {
                SaveEntry_St* e = &saveList.items[i];

                bool pressed = textButtonUpdate(&e->button, mouseScreen);
                anySaveSelected = anySaveSelected || pressed;
                if (pressed) {
                    selectedSaveIndex = i;
                    e->button.textColor = YELLOW;
                }

                if (imageButtonUpdate(&e->delButton, mouseScreen)) {
                    pendingDeleteIndex = i;
                    polyBlast_currentPrompt = PROMPT_CONFIRM_DELETE;

                    return shouldCloseWindow;
                }
            }

            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && !anySaveSelected) {
                if (CheckCollisionPointRec(mouseScreen, saveListScroll.visibleArea)) {
                    if (selectedSaveIndex != -1) {
                        saveList.items[selectedSaveIndex].button.textColor = WHITE;
                    }
                    selectedSaveIndex = -1;
                }
            }
        } break;

        case PROMPT_CONFIRM_DELETE: {
            if (textButtonUpdate(&btnYesDelete, mouseScreen)) {
                const char* filename = saveList.items[pendingDeleteIndex].filename;
                if (deleteSave(filename)) {
                    log_info("Deleted save: %s", filename);
                }
                listSaves(&saveList);
                polyBlast_currentPrompt = PROMPT_SAVES_LIST;
            }

            if (textButtonUpdate(&btnNoDelete, mouseScreen)) {
                polyBlast_currentPrompt = PROMPT_SAVES_LIST;
            }
        } break;

        default: break;
    }

    if (IsKeyPressed(KEY_ESCAPE)) shouldCloseWindow = true;

    return shouldCloseWindow;
}

void polyBlast_promptDraw(void) {
    if (polyBlast_currentPrompt == PROMPT_NONE) return;

    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(BLACK, 0.7f));

    switch (polyBlast_currentPrompt) {
        case PROMPT_START_LOAD: {
            polyBlast_drawText("Do you want to load a previous save?", polyBlast_fonts[FONT48], 36,
                     (f32Vector2){WINDOW_WIDTH / 2.0f, 200}, WHITE);
            textButtonDraw(&btnNewGame, polyBlast_fonts[FONT24], 24);
            textButtonDraw(&btnLoadSave, polyBlast_fonts[FONT24], 24);
        } break;

        case PROMPT_SAVE_QUIT: {
            polyBlast_drawText("Do you want to save before leaving?", polyBlast_fonts[FONT48], 36,
                     (f32Vector2){WINDOW_WIDTH / 2.0f, 200}, WHITE);
            textButtonDraw(&btnYesSave, polyBlast_fonts[FONT24], 24);
            textButtonDraw(&btnNoSave, polyBlast_fonts[FONT24], 24);
        } break;

        case PROMPT_SAVE_FILENAME: {
            // Filename input
            polyBlast_drawText("Enter filename to save:", polyBlast_fonts[FONT32], 32,
                        (f32Vector2){WINDOW_WIDTH / 2.0f, 220}, WHITE);
            textBoxDraw(&filenameBox, polyBlast_fonts[FONT24], 24);
            textButtonDraw(&btnSaveFile, polyBlast_fonts[FONT24], 24);
            textButtonDraw(&btnCancelFile, polyBlast_fonts[FONT24], 24);
        } break;

        case PROMPT_SAVES_LIST: {
            polyBlast_drawText("Load a previous save", polyBlast_fonts[FONT48], 36,
                        (f32Vector2){WINDOW_WIDTH / 2.0f, 120}, WHITE);

            DrawTextEx(polyBlast_fonts[FONT24], "                 ------Filename------     Score     Streak Status", (Vector2){140, 170}, 24, 1, LIGHTGRAY);

            DrawRectangleRounded(saveListScroll.visibleArea, saveListScroll.roundness, 2, GetColor(0x181818FF));
            DrawRectangleRoundedLinesEx(saveListScroll.visibleArea, saveListScroll.roundness, 2, 3, GetColor(0x272727FF));

            scrollFrameBegin(&saveListScroll); {
                for (u32 i = 0; i < saveList.count; ++i) {
                    SaveEntry_St* e = &saveList.items[i];

                    textButtonDraw(&e->button, polyBlast_fonts[FONT24], 24);
                    imageButtonDraw(&e->delButton, true);
                }
            } scrollFrameEnd();

            // Bottom buttons
            textButtonDraw(&btnCancelLoad, polyBlast_fonts[FONT32], 28);

            // Grey out Load button if no saves
            btnLoadChosen.baseColor = (saveList.count > 0 && selectedSaveIndex != -1) ? BLUE : DARKGRAY;
            textButtonDraw(&btnLoadChosen, polyBlast_fonts[FONT32], 28);
        } break;

        case PROMPT_CONFIRM_DELETE: {
            polyBlast_drawText("Delete this save?", polyBlast_fonts[FONT48], 36,
                     (f32Vector2){WINDOW_WIDTH/2.0f, 240}, WHITE);
            textButtonDraw(&btnYesDelete, polyBlast_fonts[FONT32], 28);
            textButtonDraw(&btnNoDelete, polyBlast_fonts[FONT32], 28);
        } break;


        default: break;
    }
}