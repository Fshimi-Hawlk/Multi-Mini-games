/**
    @file utils/userTypes.h
    @author LeandreB8
    @date 2026-01-12
    @date 2026-03-20
    @brief Core type definitions used throughout the game - especially lobby and mini-game integration.

    Contributors:
    - LeandreB8:
        - Moved `Player_St` and `Platform_St` here
    - Fshimi-Hawlk:
        - Added documentation
        - Added `GameScene_Et`, `PlayerTextureId_Et`, `PlayerVisuals_St`, 
          `MiniGameManager_St` and `LobbyGame_St` to centralize logic and 
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
#include "APIs/chatAPI.h"
#include "networkInterface.h"

typedef enum {
    GAME_STATE_GAMEPLAY,
    GAME_STATE_CONNECTION,
} GameState_Et;

/**
    @brief Available player avatar/skin textures that can be selected in the lobby.
*/
typedef enum {
    PLAYER_TEXTURE_DEFAULT,
    PLAYER_TEXTURE_EARTH,
    PLAYER_TEXTURE_TROLL_FACE,
    PLAYER_TEXTURE_BATTLESHIP_TODO,     //< a ship -> not round
    PLAYER_TEXTURE_BINGO_TODO,          //< bingo card (?) -> not round or the ball (?)
    PLAYER_TEXTURE_CONNECT_4_TODO,      //< don't know
    PLAYER_TEXTURE_KFF_TODO,            //< king for four card (?) -> not round
    PLAYER_TEXTURE_MINIGOLF_TODO,       //< don't know
    PLAYER_TEXTURE_MORPION_TODO,        //< don't know
    PLAYER_TEXTURE_OTHELLO_TODO,        //< don't know
    __playerTextureCount,
} PlayerTextureId_Et;

enum {
    ACTION_CODE_LOBBY_MOVE = firstAvailableActionCode,
    ACTION_CODE_LOBBY_ROOM_QUERY,
    ACTION_CODE_LOBBY_ROOM_INFO,
    ACTION_CODE_LOBBY_CHAT,
    ACTION_CODE_LOBBY_SWITCH_GAME
};

/**
    @brief Visual / rendering related state of the player character.

    Keeps texture handles and UI-related flags separate from physics state.
*/
typedef struct {
    Rectangle defaultTextureRect;               ///< Source rectangle used when no special animation/state is active
    bool      isTextureMenuOpen;                ///< Whether the skin/character selection overlay is currently visible
    Texture   textures[__playerTextureCount];   ///< Preloaded textures for each available player skin
    // Future extension point: Rectangle currentSourceRect; AnimationState animation; etc.
} PlayerVisuals_St;

/**
    @brief Physics and movement state of the player character in the lobby (platformer).

    Most fields are directly used / modified by the player controller system.
*/
typedef struct {
    bool    active;

    Vector2 position;                           ///< Center position of the player (world coordinates)
    float   radius;                             ///< Collision radius (circle-based collision)

    float   angle;                              ///< Visual rotation in radians (usually 0 unless doing tricks/rotations)
    PlayerTextureId_Et textureId;               ///< Currently selected / active skin
    bool    unlockedTextures[__playerTextureCount]; ///< Which skins the player has already unlocked

    Vector2 velocity;                           ///< Current movement speed (pixels per second)

    bool    onGround;                           ///< True when player is standing on a platform (affects jump eligibility)
    int     nbJumps;                            ///< Number of jumps performed since last grounded state (multi-jump tracking)

    float   coyoteTime;                         ///< How many seconds player can still jump after leaving ground (coyote time)
    float   coyoteTimer;                        ///< Countdown timer for coyote time

    float   jumpBuffer;                         ///< Remaining time window to accept jump input before landing (jump buffering)
} Player_St;

/**
    @brief One piece of terrain in the lobby world.
    Extended from the original Platform_St to support the different zone types you requested.
*/
typedef struct {
    Rectangle      rect;                  ///< World position and size
    Color          color;                 ///< Debug / base rendering color
    float          roundness;             ///< 0.0f = sharp corners, 1.0f = fully rounded
} LobbyTerrain_St;

typeDA(LobbyTerrain_St, TerrainVec_St);

typedef struct {
    Rectangle hitbox;
    const char name[256];
} GameInteractionZone_St;

/**
    @brief Complete state of the lobby / main hub world.
*/
typedef struct{
    BaseGame_St         base;
    GameState_Et        currentState;               ///< Current state of the game.

    Chat_St             chat;                       ///< Game chat
    Player_St           otherPlayers[MAX_CLIENTS];  ///< Array of other players in the lobby.
    Player_St           player;                     ///< Physics & movement state of the player character
    PlayerVisuals_St    playerVisuals;              ///< Rendering and skin selection state
    Camera2D            cam;                        ///< 2D camera following the player
} LobbyGame_St;

#endif // USER_TYPES_H
