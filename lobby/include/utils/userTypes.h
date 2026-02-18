/**
 * @file userTypes.h
 * @author 
 * @date 
 * @brief Core type definitions used throughout the game.
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
   __gameSceneCount // can be removed/commented out if not needed
} GameScene_Et;

typedef enum {
    PLAYER_TEXTURE_DEFAULT,
    PLAYER_TEXTURE_EARTH,
    PLAYER_TEXTURE_TROLL_FACE,
    __playerTextureCount,
} PlayerTexture_Et;

typedef struct {
    Rectangle defaultTextureRect;
    PlayerTexture_Et textureId;
    Texture textures[__playerTextureCount];
    // possibly future: Rectangle currentSourceRect; or animation state
} PlayerVisuals_St;

typedef struct {
    Vector2 position;
    float radius;

    float angle;
    PlayerVisuals_St visuals;

    Vector2 velocity;
    
    bool onGround;
    int nbJumps;

    float coyoteTime;
    float coyoteTimer;

    float jumpBuffer;
} Player_st;

typedef struct {
    Rectangle rect;
    Color color;
    float roundness;
} Platform_st;

typedef struct {
    Rectangle gameHitboxes[__gameSceneCount];
    Game_St* miniGames[__gameSceneCount];

    GameScene_Et currentScene;

    bool needGameInit;
    bool gameHitGracePeriodActive;
} SubGameManager_St;

typedef struct {
    bool running;
    bool paused;                ///< Whether the game is paused or not

    Player_st player;
    Camera2D cam;

    SubGameManager_St subGameManager;
} LobbyGame_St;

#endif // USER_TYPES_H