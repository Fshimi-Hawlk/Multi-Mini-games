/**
    @file utils/userTypes.h
    @author LeandreB8
    @date 2026-01-12
    @date 2026-03-30
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
    GAME_STATE_ROOM_LIST,
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
    @brief Runtime-editable physics & water parameters for the lobby.
*/
typedef struct {
    f32 gravity;
    f32 moveSpeed;
    f32 jumpForce;
    f32 coyoteTime;
    f32 jumpBufferTime;
    s32 maxJumps;
    f32 friction;
    f32 iceFriction;

    f32  waterBuoyancy;
    f32  waterDefaultSink;
    f32  waterSinkWithS;
    f32  waterHorizDrag;
    f32  waterVertDrag;
    f32  waterJumpForce;
    f32  waterTargetSubmersion;
    f32  waterMaxSubmersion;
    bool waterCanJump;
    bool waterInfiniteJump;
    bool waterAlwaysFloat;
} PhysicsConstants_St;

/**
    @brief Visual / rendering related state of the player character.
*/
typedef struct {
    Rectangle defaultTextureRect;
    bool      isTextureMenuOpen;
    Texture   textures[__playerTextureCount];
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
    @brief Physics and movement state of the player character in the lobby.
*/
typedef struct {
    bool    active;
    char    name[32];

    Vector2 position;
    f32     radius;
    f32     angle;

    PlayerTextureId_Et textureId;
    bool    unlockedTextures[__playerTextureCount];

    Vector2 velocity;
    bool    onGround;
    s32     nbJumps;

    f32     coyoteTime;
    f32     coyoteTimer;
    f32     jumpBuffer;

    bool    isInWater;
    bool    waterFastDescent;
    bool    onIce;
    f32     portalTeleportCooldown;

    Vector2 targetPosition; // Network sync
} Player_St;

/**
    @brief Terrain/platform types in the lobby world.
*/
typedef enum {
    TERRAIN_NORMAL,
    TERRAIN_WOOD,
    TERRAIN_STONE,
    TERRAIN_ICE,
    TERRAIN_BOUNCY,
    TERRAIN_MOVING_H,
    TERRAIN_MOVING_V,
    TERRAIN_WATER,
    TERRAIN_DECORATIVE,
    TERRAIN_PORTAL,
    __terrainTypeCount
} TerrainType_Et;

/**
    @brief One piece of terrain in the lobby world.
*/
typedef struct {
    Rectangle      rect;
    Color          color;
    f32            roundness;
    TerrainType_Et type;

    Vector2        velocity;
    f32            moveDistance;

    Vector2        portalTargetPosition;
    bool           isTwoWayPortal;
    bool           isOnlyReceiverPortal;
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
    GameState_Et        currentState;

    s32                 id;

    Chat_St             chat;
    Player_St           otherPlayers[MAX_CLIENTS];
    Player_St           player;
    PlayerVisuals_St    playerVisuals;
    Camera2D            cam;

    // Editor mode
    bool                editorMode;
    bool                showLeftPalette;
    bool                showPropertiesPanel;
    s32                 selectedTerrainIndex;
    bool                isDragging;
    Vector2             dragStartWorld;
    bool                showGrid;
    f32                 gridStep;

    // Live physics
    PhysicsConstants_St physics[__playerTextureCount];
    s32                 physicsPanelEditIndex;
    char                physicsPanelEditBuffer[64];
    u32                 physicsPanelEditCursor;
} LobbyGame_St;

#endif // USER_TYPES_H
