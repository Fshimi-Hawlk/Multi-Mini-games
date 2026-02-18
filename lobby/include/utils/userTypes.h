/**
 * @file userTypes.h
 * @author i-Charlys (CAILLON Charles)
 * @date 2026-03-18
 * @brief Core type definitions used throughout the game.
 */

#ifndef USER_TYPES_H
#define USER_TYPES_H

#include "common.h"
#include "APIs/generalAPI.h"

/**
 * @brief Enum for font sizes.
 */
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
   __gameSceneCount
} GameScene_Et;

/**
 * @brief Enum for different game states in the lobby.
 */
typedef enum { 
    GAME_STATE_CONNECTION, ///< Initial connection state.
    GAME_STATE_LOBBY      ///< Active lobby state.
} GameState;

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
    Vector2 position;     ///< Current position of the player.

    float radius;
    PlayerVisuals_St visuals;

    float angle;          ///< Current rotation angle of the player.
    Vector2 velocity;     ///< Current velocity vector.
    bool onGround;        ///< Flag indicating if the player is touching the ground.
    int nbJumps;          ///< Current number of jumps performed.
    float coyoteTime;     ///< Duration of coyote time.
    float coyoteTimer;    ///< Timer for coyote time.
    float jumpBuffer;     ///< Timer for jump buffering.
    bool active;          ///< Flag indicating if the player is active.
} Player_st;

/**
 * @brief Structure representing a platform.
 */
typedef struct {
    Rectangle rect;       ///< Rectangle defining the platform bounds.
    Color color;          ///< Color of the platform.
    float roundness;      ///< Roundness of the platform corners.
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