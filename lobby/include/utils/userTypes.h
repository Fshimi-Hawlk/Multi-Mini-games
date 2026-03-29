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
        - Added `isInWater`, `waterFastDescent` and `onIce` to `Player_St` for the new terrain physics.

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
    @brief Runtime-editable physics & water parameters for the lobby.
           All values can be live-tuned via the F2 debug panel.
           Changes take effect immediately on the player.
*/
typedef struct {
    // ── Land / general ─────────────────────────────────────────────────────
    f32 gravity;            ///< Downward acceleration (pixels/second²)
    f32 moveSpeed;          ///< Horizontal speed when holding A/D
    f32 jumpForce;          ///< Upward impulse on land jump
    f32 coyoteTime;         ///< Coyote-time window (seconds)
    f32 jumpBufferTime;     ///< Jump-buffer window (seconds)
    s32 maxJumps;           ///< Maximum air jumps allowed
    f32 friction;           ///< Horizontal deceleration on normal ground
    f32 iceFriction;        ///< Horizontal deceleration on ice

    // ── Water base ─────────────────────────────────────────────────────────
    f32  waterBuoyancy;         ///< Base upward force when submerged
    f32  waterDefaultSink;      ///< Gentle auto-sink force
    f32  waterSinkWithS;        ///< Sink force when holding S
    f32  waterHorizDrag;        ///< X drag multiplier in water
    f32  waterVertDrag;         ///< Y drag multiplier in water
    f32  waterJumpForce;        ///< Reduced jump force while in water
    f32  waterTargetSubmersion; ///< Desired submersion ratio for floating skins (0–1)
    f32  waterMaxSubmersion;    ///< If exceeded, buoyancy is disabled (battleship)
    bool waterCanJump;          ///< Can the player jump at all in water?
    bool waterInfiniteJump;     ///< Can the player keep jumping indefinitely in water?
    bool waterAlwaysFloat;      ///< Strong buoyancy even when fully submerged (e.g. EARTH)
} PhysicsConstants_St;

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

    bool    isInWater;                          ///< True when player circle overlaps any TERRAIN_WATER rect this frame
    bool    waterFastDescent;                   ///< Ctrl+S toggle: fast descent + no buoyancy (cannot float back up)
    bool    onIce;                              ///< True when currently standing on TERRAIN_ICE (enables slippery friction)
} Player_St;

/**
    @brief Terrain/platform types in the lobby world.
    Each type can trigger different gameplay effects (movement, friction, hazards, visuals).
    @note Effects are implemented in the lobby update/render functions – this struct only stores the data.
*/
typedef enum {
    TERRAIN_NORMAL,      ///< Standard solid ground – no special effect
    TERRAIN_WOOD,        ///< Wooden aesthetic (slight sound variation)
    TERRAIN_STONE,       ///< Stone/rock look (hard surface)
    TERRAIN_ICE,         ///< Slippery – reduce player friction
    TERRAIN_BOUNCY,      ///< Jump pad – gives extra upward impulse on contact
    TERRAIN_MOVING_H,    ///< Moves horizontally (ping-pong within moveDistance)
    TERRAIN_MOVING_V,    ///< Moves vertically (ping-pong within moveDistance)
    TERRAIN_WATER,       ///< Shallow water zone – visual splash, optional slow-down
    TERRAIN_DECORATIVE,  ///< Visual only (trees, bushes, signs) – no collision
    TERRAIN_PORTAL,      ///< Teleport zone (destination stored in portalTargetPosition)
    __terrainTypeCount
} TerrainType_Et;

/**
    @brief One piece of terrain in the lobby world.
    Extended from the original Platform_St to support the different zone types you requested.
*/
typedef struct {
    Rectangle      rect;                  ///< World position and size
    Color          color;                 ///< Debug / base rendering color
    float          roundness;             ///< 0.0f = sharp corners, 1.0f = fully rounded
    TerrainType_Et type;                  ///< Determines collision, movement, and effects
    Vector2        velocity;              ///< Speed/direction for moving types (0 otherwise)
    float          moveDistance;          ///< Oscillation distance for moving types (0 otherwise)
    Vector2        portalTargetPosition;  ///< Where the player is teleported to
    bool           isTwoWayPortal;        ///< True = bidirectional, false = one-way
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

    // ── Editor mode ─────────────────────────────────────────────────────────
    /**
        @brief Editor-specific state. Only used when editorMode is active.
    */
    bool                editorMode;                 ///< True = editor active, game paused
    bool                showLeftPalette;            ///< Show left terrain palette sidebar
    bool                showPropertiesPanel;        ///< Show right properties panel

    s32                 selectedTerrainIndex;       ///< -1 = nothing selected
    bool                isDragging;                 ///< Currently dragging a selection
    Vector2             dragStartWorld;             ///< World position where drag started

    bool                showGrid;                   ///< Toggle grid visibility (future)
    f32                 gridStep;                   ///< Grid size in world units (future)

    // ── Live physics constants (F2 debug panel) ─────────────────────────────
    /**
        @brief Physics constants for each player skin.
               Indexed by `PlayerTextureId_Et`.
               Initialized in `lobby_init()` from the old `getWaterBehaviour()` logic.
    */
    PhysicsConstants_St physics[__playerTextureCount];

    s32                 physicsPanelEditIndex;      ///< -1 = not editing, else index of edited constant
    char                physicsPanelEditBuffer[64]; ///< Temporary buffer for live editing
    u32                 physicsPanelEditCursor;     ///< Cursor position in edit buffer
} LobbyGame_St;

#endif // USER_TYPES_H
