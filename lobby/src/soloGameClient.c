/**
 * @file soloGameClient.c
 * @brief Wrappers pour lancer les jeux solo comme processus séparés.
 */

#include "ui/menus.h"
#include "setups/app.h"
#include "utils/globals.h"
#include <stdlib.h>

static bool soloGameRunning = false;

static void soloGameInit(void) {
    #ifdef _WIN32
    int ret = system("start games\\mini-golf\\build\\bin\\main.exe");
    (void)ret;
    #else
    int ret = system("./games/mini-golf/build/bin/main &");
    (void)ret;
    #endif
    soloGameRunning = true;
}

static void soloGameUpdate(float dt) {
    (void)dt;
}

static void soloGameDraw(void) {
    int sw = GetScreenWidth();
    int sh = GetScreenHeight();
    DrawRectangleGradientV(0, 0, sw, sh, DARKBLUE, BLACK);
    DrawTextEx(
        lobby_fonts[FONT32], "Mini Golf",
        (Vector2){sw / 2.0f - 80, sh / 2.0f - 60}, 32, 0, GOLD
    );
    DrawTextEx(
        lobby_fonts[FONT24], "Jeu lance dans une fenetre separee.",
        (Vector2){sw / 2.0f - 200, sh / 2.0f}, 22, 0, WHITE
    );
    DrawTextEx(
        lobby_fonts[FONT24], "Appuyez sur ECHAP pour revenir au lobby.",
        (Vector2){sw / 2.0f - 220, sh / 2.0f + 40}, 22, 0, LIGHTGRAY
    );
}

static void soloGameDestroy(void) {
    soloGameRunning = false;
}

static GameClientInterface_St miniGolfClientInterface = {
    .id      = MINI_GAME_ID_MINI_GOLF,
    .name    = "Mini Golf",
    .init    = soloGameInit,
    .onData = NULL,
    .update  = soloGameUpdate,
    .draw    = soloGameDraw,
    .destroy = soloGameDestroy
};

extern void setCurrentMiniGame(GameClientInterface_St* iface);

void launchSoloGame(u8 gameId) {
    switch (gameId) {
    case MINI_GAME_ID_MINI_GOLF:
        // setCurrentMiniGame appelle deja init() — NE PAS rappeler soloGameInit() ici
        setCurrentMiniGame(&miniGolfClientInterface);
        log_info("Lance Mini Golf");
        break;
    default:
        log_warn("launchSoloGame: jeu non supporte ID=%d", gameId);
        break;
    }
}