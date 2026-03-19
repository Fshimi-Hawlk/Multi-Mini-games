/**
 * @file lobbyAPI.c
 * @author Maxime CHAUVEAU
 * @date 2026-03-18
 * @brief Lobby implementation for RubiksCube game selection
 */

#include "lobbyAPI.h"
#include <stdio.h>
#include <stdlib.h>

struct LobbyGame_St {
    bool running;
    int selectedGame;
    uint32_t fps;
};

Error_Et lobby_initGame__full(LobbyGame_St** game, LobbyConfigs_St configs) {
    if (game == NULL) {
        return ERROR_INIT;
    }
    
    *game = (LobbyGame_St*)malloc(sizeof(LobbyGame_St));
    if (*game == NULL) {
        return ERROR_ALLOC;
    }
    
    (*game)->running = true;
    (*game)->selectedGame = 0;
    (*game)->fps = configs.fps > 0 ? configs.fps : 60;
    
    return OK;
}

Error_Et lobby_update(LobbyGame_St* const game) {
    if (game == NULL) {
        return ERROR_INIT;
    }
    
    return OK;
}

Error_Et lobby_render(const LobbyGame_St* const game) {
    if (game == NULL) {
        return ERROR_INIT;
    }
    
    return OK;
}

void lobby_freeGame(LobbyGame_St* game) {
    if (game != NULL) {
        free(game);
    }
}
