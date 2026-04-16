/**
    @file userTypes.h
    @author Léandre BAUDET
    @date 2026-01-12
    @date 2026-04-14
    @brief Core type definitions used throughout the game - especially lobby and mini-game integration.
*/
#ifndef USER_TYPES_H
#define USER_TYPES_H

#include "common.h"

/**
    @brief Definition of typedef struct
*/
typedef struct {
    Vector2 position;
    f32 height;
    f32 angle;
    f32 velocity;
    Color color;
} GrassBlade_St;

/**
    @brief Definition of typedef enum
*/
typedef enum {
    FIREFLY_MODE_WANDER,
    FIREFLY_MODE_LOOP,
    FIREFLY_MODE_BOB
} FireflyMode_Et;

/**
    @brief Definition of typedef struct
*/
typedef struct {
    Vector2 position;
    Vector2 velocity;
    f32   radius;
    f32   alpha;
    f32   phase;
    bool    active;

    // Enhanced behavior
    FireflyMode_Et mode;
    f32          modeTimer;
    Vector2        wanderTarget;
    Vector2        loopPoints[12];
    int            loopCount;
    int            currentLoopIndex;
    f32          facingAngle;
    f32          currentSpeed;
} Firefly_St;

/**
    @brief Definition of typedef struct
*/
typedef struct {
    Vector2 position;
    Vector2 velocity;
    f32   rotation;
    f32   rotationSpeed;
    f32   scale;
    f32   life;
    f32   currentAlpha;
    bool    active;
    bool    onGround;
    f32   groundTimer;
    f32   spinDampTimer;      ///< Time left to apply strong rotational drag after player push (0 = normal drag)
    Color   color;
} FallingLeaf_St;

/**
    @brief Definition of typedef enum
*/
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
    @brief Definition of typedef enum
*/
typedef enum {
    PLAYER_TEXTURE_DEFAULT,
    PLAYER_TEXTURE_BINGO,
    PLAYER_TEXTURE_BOWLING,
    PLAYER_TEXTURE_CHESS,
    PLAYER_TEXTURE_DROP_FOUR,
    PLAYER_TEXTURE_TWIST_CUBE,
    PLAYER_TEXTURE_EDITOR,      ///< Level editor integrated as a mini-game
    PLAYER_TEXTURE_KING_FOR_FOUR,
    PLAYER_TEXTURE_LOBBY,       ///< Main lobby / hub world with platformer movement
    PLAYER_TEXTURE_MINI_GOLF,
    PLAYER_TEXTURE_DISC_REVERSAL,
    PLAYER_TEXTURE_POLY_BLAST,
    PLAYER_TEXTURE_SNAKE,
    PLAYER_TEXTURE_SOLO_CARDS,
    PLAYER_TEXTURE_SUIKA,
    PLAYER_TEXTURE_TETROMINO_FALL,
    __playerTextureCount,
} PlayerTextureId_Et;

/**
    @brief Runtime-editable physics & water parameters for the lobby.
*/
typedef struct {
    f32 moveSpeed;

    f32 gravity;
    f32 maxFallSpeed;
    f32 airDrag;

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
    TERRAIN_KIND_NORMAL,
    TERRAIN_KIND_GRASS,
    TERRAIN_KIND_WOOD_PLANK,
    TERRAIN_KIND_STONE,
    TERRAIN_KIND_ICE,
    TERRAIN_KIND_BOUNCY,
    TERRAIN_KIND_MOVING_H,
    TERRAIN_KIND_MOVING_V,
    TERRAIN_KIND_WATER,
    TERRAIN_KIND_DECORATIVE,
    TERRAIN_KIND_PORTAL,
    __terrainKindCount
} TerrainKind_Et;

/**
    @brief One piece of terrain in the lobby world.
*/
typedef struct {
    Rectangle      rect;
    Color          color;
    f32            roundness;
    TerrainKind_Et kind;

    Vector2        velocity;
    f32            moveDistance;

    Vector2        portalTargetPosition;
    bool           isTwoWayPortal;
    bool           isOnlyReceiverPortal;
} LobbyTerrain_St;

/**
    @brief Description for typeDA
    @return Success/failure or the result of the function
*/
typeDA(LobbyTerrain_St, TerrainVec_St);

/**
    @brief One clickable zone that leads to a mini-game from the lobby.
           Name is a fixed buffer to make binary save/load trivial and allocation-free.
*/
typedef struct {
    Rectangle hitbox;
    char      name[32];
    Color     color;
    bool      active;
    bool      isRestricted;
} GameInteractionZone_St;

/**
    @brief Complete state of the lobby / main hub world.
*/
typedef struct {
    BaseGame_St         base;
    GameState_Et        currentState;

    s32                 clientId;

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
