#include "core/game.h"
#include "core/audio.h"

#include "networkInterface.h"
#include "paramsMenu.h"
#include "APIs/generalAPI.h"


static SuikaGame_St suika_game = {0};
static ParamsMenu_St suika_paramsMenu = {0};

static bool seeded = false;

void suika_init(void) {
    if (!seeded) {
        srand((unsigned int)time(NULL));
        seeded = true;
    }

    memset(&suika_game, 0, sizeof(*&suika_game));

    suika_loadAssets(&suika_game);
    suika_initAudio();
    suika_initGame(&suika_game);

    paramsMenu_init(&suika_paramsMenu);
}

void suika_update(float dt) {
    paramsMenu_update(&suika_paramsMenu);
    suika_updateGame(&suika_game, dt);
}

void suika_draw(void) {
    suika_drawGame(&suika_game);
    paramsMenu_draw(&suika_paramsMenu);
}

void suika_destroy(void) {
    paramsMenu_free(&suika_paramsMenu);

    suika_unloadAssets(&suika_game);
    suika_freeAudio();
}

GameClientInterface_St suika_clientInterface = {
    .id = MINI_GAME_ID_SUIKA,
    .name = "Suika",
    .init = suika_init,
    .onData = NULL,
    .update = suika_update,
    .draw = suika_draw,
    .destroy = suika_destroy
};