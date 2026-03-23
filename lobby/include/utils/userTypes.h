/**
    @file utils/userTypes.h
    @author LeandreB8
    @date 2026-01-12
    @date 2026-02-23
    @brief Core type definitions used throughout the game.
*/

#ifndef USER_TYPES_H
#define USER_TYPES_H

#include "common.h"
#include "APIs/generalAPI.h"

typedef enum {
    FONT8,
    FONT10, FONT12, FONT14, FONT16, FONT18,
    FONT20, FONT22, FONT24, FONT26, FONT28,
    FONT30, FONT32, FONT34, FONT36, FONT38,
    FONT40, FONT42, FONT44, FONT46, FONT48,
    _fontSizeCount
} FontSize_Et;

typedef enum {
    GAME_SCENE_LOBBY,
    GAME_SCENE_TETRIS,
    GAME_SCENE_SOLITAIRE,
    GAME_SCENE_SUIKA,
    GAME_SCENE_BOWLING,
    __gameSceneCount
} GameScene_Et;

typedef enum {
    PLAYER_TEXTURE_DEFAULT,
    PLAYER_TEXTURE_EARTH,
    PLAYER_TEXTURE_TROLL_FACE,
    __playerTextureCount,
} PlayerTextureId_Et;

typedef struct {
    Rectangle defaultTextureRect;
    bool      isTextureMenuOpen;
    Texture   textures[__playerTextureCount];
} PlayerVisuals_St;

typedef struct {
    Vector2 position;                           ///< Player center (world coordinates)
    float   radius;

    float   angle;
    PlayerTextureId_Et textureId;
    bool    unlockedTextures[__playerTextureCount];

    Vector2 velocity;

    bool    onGround;
    int     nbJumps;

    /* FIX: coyoteTime field removed — it was initialized in lobbyAPI.c but never
     * read afterwards: the constant COYOTE_TIME (configs.h) is used everywhere
     * in game.c. Keeping both was confusing. */
    float   coyoteTimer;

    float   jumpBuffer;
} Player_st;

typedef struct {
    Rectangle rect;
    Color     color;
    float     roundness;
} Platform_st;

typedef struct {
    Rectangle     gameHitboxes[__gameSceneCount];
    BaseGame_St*  miniGames[__gameSceneCount];

    GameScene_Et  currentScene;

    bool          needGameInit;
    bool          gameHitGracePeriodActive;
} SubGameManager_St;

typedef struct {
    BaseGame_St base;

    Player_st         player;
    PlayerVisuals_St  playerVisuals;
    Camera2D          cam;

    SubGameManager_St subGameManager;
} LobbyGame_St;

#endif // USER_TYPES_H
