/**
    @file utils/userTypes.h
    @author Fshimi Hawlk
    @author LeandreB8
    @date 2026-01-12
    @date 2026-03-18
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
    @brief Identifiers of the different playable scenes / mini-games.

    Used both as array indices and as state identifiers.
*/
typedef enum {
    GAME_SCENE_LOBBY,       ///< Main lobby / hub world with platformer movement
    GAME_SCENE_TETRIS,      ///< Tetris mini-game
    __gameSceneCount
} GameScene_Et;

/**
    @brief Enum for different game states in the lobby.
*/
typedef enum { 
    LOBBY_STATE_CONNECTION, ///< Initial connection state.
    LOBBY_STATE_LOBBY      ///< Active lobby state.
} LobbyState_Et;

/**
    @brief Available player avatar/skin textures that can be selected in the lobby.
*/
typedef enum {
    PLAYER_TEXTURE_DEFAULT,
    PLAYER_TEXTURE_EARTH,
    PLAYER_TEXTURE_TROLL_FACE,
    __playerTextureCount,
} PlayerTextureId_Et;

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
    bool    active;                             ///< Whether this player is active/connected
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
    Vector2 targetPosition;                     ///< Network target position for smoothing
} Player_st;

/**
    @brief Single rectangular platform / solid surface in the lobby world.
*/
typedef struct {
    Rectangle rect;         ///< Position and size (world coordinates)
    Color     color;        ///< Debug / placeholder rendering color
    float     roundness;    ///< Corner roundness factor (0 = sharp, 1 = fully round)
} Platform_st;

/**
    @brief Manages which mini-game is currently active and its integration with the lobby.

    Acts as a lightweight scene manager / sub-game router.
*/
typedef struct {
    Rectangle     gameHitboxes[__gameSceneCount];   ///< Screen-space rectangles where touching/standing activates a mini-game
    BaseGame_St*  miniGames[__gameSceneCount];      ///< Pointers to the actual mini-game state objects (may be NULL)

    GameScene_Et  currentScene;                     ///< Which mini-game / view is currently active

    bool          needGameInit;                     ///< Flag: newly selected mini-game needs initialization on next frame
    bool          gameHitGracePeriodActive;         ///< Prevents instant re-triggering when leaving/entering hitbox
} SubGameManager_St;

/**
    @brief Complete state of the lobby / main hub world.
*/
typedef struct {
    BaseGame_St base;

    Player_st         player;                     ///< Physics & movement state of the player character
    Player_st         otherPlayers[8];            ///< Other players connected to the lobby (MAX_CLIENTS = 8)
    PlayerVisuals_St  playerVisuals;              ///< Rendering and skin selection state
    Camera2D          cam;                        ///< 2D camera following the player

    SubGameManager_St subGameManager;           ///< Manages transitions to/from mini-games
} LobbyGame_St;

#endif // USER_TYPES_H
