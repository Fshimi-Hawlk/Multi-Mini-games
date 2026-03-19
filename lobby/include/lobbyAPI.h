/**
 * @file lobbyAPI.h
 * @author Maxime CHAUVEAU
 * @date 2026-03-18
 * @brief Public API for the RubiksCube Lobby
 */

#ifndef LOBBY_API_H
#define LOBBY_API_H

#include <stdint.h>
#include <stdbool.h>

typedef enum {
    OK = 0,
    ERROR_ALLOC,
    ERROR_INIT
} Error_Et;

typedef struct {
    uint32_t fps;
} LobbyConfigs_St;

typedef struct LobbyGame_St LobbyGame_St;

#define lobby_initGame(game, ...) \
    lobby_initGame__full(game, (LobbyConfigs_St){ __VA_ARGS__ })

Error_Et lobby_initGame__full(LobbyGame_St** game, LobbyConfigs_St configs);
Error_Et lobby_update(LobbyGame_St* const game);
Error_Et lobby_render(const LobbyGame_St* const game);
void lobby_freeGame(LobbyGame_St* game);

#endif // LOBBY_API_H
