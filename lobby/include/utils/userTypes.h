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

typedef struct {
    Vector2 position;
    float height;
    float angle;      // L'angle actuel du brin
    float velocity;   // La vitesse de rotation (pour l'élasticité)
    Color color;
} GrassBlade_St;

typedef enum {
    FIREFLY_MODE_WANDER,
    FIREFLY_MODE_LOOP,
    FIREFLY_MODE_BOB
} FireflyMode_Et;

typedef struct {
    Vector2 position;
    Vector2 velocity;
    float   radius;
    float   alpha;
    float   phase;
    bool    active;

    // Enhanced behavior
    FireflyMode_Et mode;
    float          modeTimer;
    Vector2        wanderTarget;
    Vector2        loopPoints[12];
    int            loopCount;
    int            currentLoopIndex;
    float          facingAngle;
    float          currentSpeed;
} Firefly_St;

typedef struct {
    Vector2 position;
    Vector2 velocity;
    float   rotation;
    float   rotationSpeed;
    float   scale;
    float   life;
    float   currentAlpha;
    bool    active;
    bool    onGround;
    float   groundTimer;
    float   spinDampTimer;      ///< Time left to apply strong rotational drag after player push (0 = normal drag)
    Color   color;
} FallingLeaf_St;

typedef enum {
    GAME_SCENE_LOBBY,
    GAME_SCENE_TETRIS,
    GAME_SCENE_SOLITAIRE,
    GAME_SCENE_SUIKA,
    GAME_SCENE_BOWLING,
    GAME_SCENE_GOLF,
    GAME_SCENE_SNAKE,
    GAME_SCENE_BINGO,
    GAME_SCENE_BLOCKBLAST,
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
    Vector2 position;
    float   radius;

    float   angle;
    PlayerTextureId_Et textureId;
    bool    unlockedTextures[__playerTextureCount];

    Vector2 velocity;

    bool    onGround;
    int     nbJumps;

    float   coyoteTimer;

    float   jumpBuffer; 
} Player_St;

typedef enum {
    PLATFORM_TYPE_GRASS,
    PLATFORM_TYPE_WOODPLANK,
    __platformTypeCount
} PlatformType_Et;

typedef struct {
    Rectangle rect;         ///< Position and size (world coordinates)
    Color     color;        ///< Debug / placeholder rendering color
    float     roundness;    ///< Corner roundness factor (0 = sharp, 1 = fully round)
    PlatformType_Et type;
} Platform_St;

typedef struct {
    Rectangle hitbox;
    const char *name;
    Color color;
} GameCollisionZone_St;

typedef struct {
    GameCollisionZone_St gameZones[__gameSceneCount];
    BaseGame_St*  miniGames[__gameSceneCount];

    GameScene_Et  currentScene;

    bool          needGameInit;
    bool          gameHitGracePeriodActive;
} SubGameManager_St;

typedef struct {
    BaseGame_St base;

    Player_St         player;                     ///< Physics & movement state of the player character
    PlayerVisuals_St  playerVisuals;              ///< Rendering and skin selection state
    Camera2D          cam;                        ///< 2D camera following the player

    SubGameManager_St subGameManager;
} LobbyGame_St;

#endif // USER_TYPES_H
