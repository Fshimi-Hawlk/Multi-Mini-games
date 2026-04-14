/**
    @file editor/clientInterface.c
    @author Fshimi-Hawlk
    @date 2026-04-14
    @brief Client interface for client.
*/

#include "editor/editor.h"
#include "utils/globals.h"


static void editor_init(void) {
    initEditor(&lobby_game);
}

static void editor_update(float dt) {
    updateEditor(&lobby_game, dt);
}

static void editor_draw(void) {
    drawEditor(&lobby_game);
}

GameClientInterface_St editorClientInterface = {
    .id      = MINI_GAME_ID_EDITOR,
    .name    = "Level Editor",
    .init    = editor_init,
    .update  = editor_update,
    .draw    = editor_draw,
};