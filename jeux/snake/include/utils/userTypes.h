/**
    @file utils/userTypes.h
    @author LeandreB8
    @date 2026-01-12
    @date 2026-02-23
    @brief Core type definitions used throughout the game - especially lobby and mini-game integration.

    Contributors:
    - LeandreB8:
        - Moved `Player_St` and `Platform_St` here
    - Fshimi-Hawlk:
        - Added documentation
        - Added `GameScene_Et`, `PlayerTextureId_Et`, `PlayerVisuals_St`, 
          `SubGameManager_St` and `LobbyGame_St` to centralize logic and 
          previously global variables and make everything as straight forward.

    This header contains the central enumerated types and data structures that describe:
        - visual appearance and state of the player in the lobby
        - platformer physics state of the lobby player
        - currently active mini-game / sub-scene
        - overall lobby game state

    Most gameplay systems in the lobby directly or indirectly depend on types defined here.
*/

#ifndef USER_TYPES_H
#define USER_TYPES_H

#include "common.h"

typedef enum {
    GAME_TILE_GRASS,
    GAME_TILE_HEAD,
    GAME_TILE_BODY,
    GAME_TILE_APPLE
} GameTile_Et;

typedef struct {
    f32 timer;
    f32 delay;
} SnakeAnimationData_St;

typedef int Board_t[SIZE_BOARD][SIZE_BOARD];

typedef struct SnakeBodyPart_St SnakeBodyPart_St;

struct SnakeBodyPart_St {
    iVector2 coord;
    Vector2 renderPos;
    SnakeBodyPart_St* suivant;
};

typedef struct {
    SnakeBodyPart_St* head;
    SnakeBodyPart_St* tail;
    int bodyLength;
} Snake_St;

#endif // USER_TYPES_H
