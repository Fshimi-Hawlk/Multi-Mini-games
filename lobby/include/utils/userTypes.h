/**
    @file utils/userTypes.h
    @author LeandreB8
    @date 2026-01-12
    @date 2026-03-20
    @brief Core type definitions used throughout the game - especially lobby and mini-game integration.
*/

#ifndef USER_TYPES_H
#define USER_TYPES_H

#include "common.h"
#include "APIs/generalAPI.h"
#include "APIs/chatAPI.h"

typedef enum {
    GAME_STATE_GAMEPLAY,
    GAME_STATE_CONNECTION,
    GAME_STATE_DISCONNECTED,
    GAME_STATE_CONNECTING,
    GAME_STATE_LOBBY,
    GAME_STATE_WAITING_SWITCH,
    GAME_STATE_INGAME,
} GameState_Et;

/**
    @brief Available player avatar/skin textures that can be selected in the lobby.
*/
typedef enum {
    PLAYER_TEXTURE_DEFAULT,
    PLAYER_TEXTURE_EARTH,
    PLAYER_TEXTURE_TROLL_FACE,
    PLAYER_TEXTURE_BATTLESHIP_TODO,
    PLAYER_TEXTURE_BINGO,
    PLAYER_TEXTURE_CONNECT_4_TODO,
    PLAYER_TEXTURE_KFF,
    PLAYER_TEXTURE_MINIGOLF_TODO,
    PLAYER_TEXTURE_MORPION_TODO,
    PLAYER_TEXTURE_OTHELLO_TODO,
    __playerTextureCount,
} PlayerTextureId_Et;

/**
    @brief Visual / rendering related state of the player character.
*/
typedef struct {
    Rectangle defaultTextureRect;               ///< Source rectangle used when no special animation/state is active
    bool      isTextureMenuOpen;                ///< Whether the skin/character selection overlay is currently visible
    Texture   textures[__playerTextureCount];   ///< Preloaded textures for each available player skin
} PlayerVisuals_St;

#pragma pack(push, 1)
/**
 * @brief Simplified player structure for network transmission.
 */
typedef struct {
    f32   x, y;
    f32   angle;
    u8    textureId;
    bool  active;
    char  name[32];
} PlayerNet_St;
#pragma pack(pop)

/**
    @brief Physics and movement state of the player character in the lobby (platformer).
*/
typedef struct {
    bool    active;
    char    name[32];

    Vector2 position;           ///< Center position of the player (world coordinates)
    f32     radius;             ///< Collision radius (circle-based collision)
    f32     angle;              ///< Visual rotation in radians

    PlayerTextureId_Et textureId;                   ///< Currently selected / active skin
    bool    unlockedTextures[__playerTextureCount]; ///< Which skins the player has already unlocked

    Vector2 velocity;           ///< Current movement speed
    bool    onGround;           ///< True when player is standing on a platform
    s32     nbJumps;            ///< Number of jumps performed

    f32     coyoteTime;         ///< Coyote time duration
    f32     coyoteTimer;        ///< Coyote time countdown
    f32     jumpBuffer;         ///< Jump buffer window
    Vector2 targetPosition;     ///< Network target position for smoothing
} Player_St;

/**
    @brief One piece of terrain in the lobby world.
*/
typedef struct {
    Rectangle      rect;                  ///< World position and size
    Color          color;                 ///< Debug / base rendering color
    f32            roundness;             ///< 0.0f = sharp corners, 1.0f = fully rounded
} LobbyTerrain_St;

typeDA(LobbyTerrain_St, TerrainVec_St);

typedef struct {
    Rectangle hitbox;
    char      name[256];
} GameInteractionZone_St;

/**
    @brief Complete state of the lobby / main hub world.
*/
typedef struct {
    BaseGame_St         base;
    GameState_Et        currentState;               ///< Current state of the game.

    s32                 id;                         ///< Internal ID (s32 for consistency)

    Chat_St             chat;                       ///< Game chat
    Player_St           otherPlayers[MAX_CLIENTS];  ///< Array of other players in the lobby.
    Player_St           player;                     ///< Physics & movement state of the player character
    PlayerVisuals_St    playerVisuals;              ///< Rendering and skin selection state
    Camera2D            cam;                        ///< 2D camera following the player
} LobbyGame_St;

#endif // USER_TYPES_H
