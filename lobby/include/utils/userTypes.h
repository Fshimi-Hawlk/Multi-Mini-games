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
#include "APIs/generalAPI.h"

/**
    @brief Available font sizes used for in-game UI and text rendering.

    Values are listed in ascending order.  
    `_fontSizeCount` is **not** a valid font size - it serves as array dimension / loop boundary.
*/
typedef enum
{
    FONT8,
    FONT10, FONT12, FONT14, FONT16, FONT18,
    FONT20, FONT22, FONT24, FONT26, FONT28,
    FONT30, FONT32, FONT34, FONT36, FONT38,
    FONT40, FONT42, FONT44, FONT46, FONT48,
    _fontSizeCount
} FontSize_Et;

/**
    @brief Identifiers of the different playable scenes / mini-games.

    Used both as array indices and as state identifiers.
*/
typedef enum
{
    GAME_SCENE_LOBBY,
    GAME_SCENE_SUIKA,
    __gameSceneCount
} GameScene_Et;

/**
    @brief Available player avatar/skin textures that can be selected in the lobby.
*/
typedef enum
{
    PLAYER_TEXTURE_DEFAULT,
    PLAYER_TEXTURE_EARTH,
    PLAYER_TEXTURE_TROLL_FACE,
    __playerTextureCount,
} PlayerTextureId_Et;

/**
    @brief Visual / rendering related state of the player character.

    Keeps texture handles and UI-related flags separate from physics state.
*/
typedef struct
{
    Rectangle defaultTextureRect;
    bool      isTextureMenuOpen;
    Texture   textures[__playerTextureCount];
} PlayerVisuals_St;

/**
    @brief Physics and movement state of the player character in the lobby (platformer).

    Most fields are directly used / modified by the player controller system.
*/
typedef struct
{
    Vector2 position;
    float   radius;

    float   angle;
    PlayerTextureId_Et textureId;
    bool    unlockedTextures[__playerTextureCount];

    Vector2 velocity;

    bool    onGround;
    int     nbJumps;

    float   coyoteTime;
    float   coyoteTimer;

    float   jumpBuffer;
} Player_st;

/**
    @brief Single rectangular platform / solid surface in the lobby world.
*/
typedef struct
{
    Rectangle rect;
    Color     color;
    float     roundness;
} Platform_st;

/**
    @brief Manages which mini-game is currently active and its integration with the lobby.

    Acts as a lightweight scene manager / sub-game router.
*/
typedef struct
{
    Rectangle     gameHitboxes[__gameSceneCount];
    BaseGame_St*  miniGames[__gameSceneCount];

    GameScene_Et  currentScene;

    bool          needGameInit;
    bool          gameHitGracePeriodActive;
} SubGameManager_St;

/**
    @brief Complete state of the lobby / main hub world.
*/
typedef struct
{
    BaseGame_St base;

    Player_st         player;
    PlayerVisuals_St  playerVisuals;
    Camera2D          cam;

    SubGameManager_St subGameManager;
} LobbyGame_St;

#endif // USER_TYPES_H
