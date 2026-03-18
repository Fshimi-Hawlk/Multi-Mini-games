/**
 * @file userTypes.h
 * @author i-Charlys (CAILLON Charles)
 * @date 2026-03-18
 * @brief Core type definitions used throughout the game.
 */

#ifndef USER_TYPES_H
#define USER_TYPES_H

#include "common.h"

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

/**
 * @brief Enum for different game scenes.
 */
typedef enum {
    GAME_SCENE_LOBBY,
    GAME_SCENE_GAME_NAME,
} GameScene_Et;

/**
 * @brief Structure representing a player.
 */
typedef struct {
    Vector2 position;     ///< Current position of the player.
    float radius;         ///< Radius of the player for physics.
    Texture2D* texture;   ///< Current texture applied to the player (local only).
    int skin_id;          ///< ID of the selected skin (synced).
    float angle;          ///< Current rotation angle of the player.
    Vector2 velocity;     ///< Current velocity vector.
    bool onGround;        ///< Flag indicating if the player is touching the ground.
    int nbJumps;          ///< Current number of jumps performed.
    float coyoteTime;     ///< Duration of coyote time.
    float coyoteTimer;    ///< Timer for coyote time.
    float jumpBuffer;     ///< Timer for jump buffering.
    bool active;          ///< Flag indicating if the player is active.
} Player_st;

#pragma pack(push, 1)
/**
 * @brief Simplified player structure for network transmission.
 */
typedef struct {
    float x, y;
    float angle;
    int skin_id;
    bool active;
} PlayerNet_st;
#pragma pack(pop)

/**
 * @brief Structure representing a platform.
 */
typedef struct {
    Rectangle rect;       ///< Rectangle defining the platform bounds.
    Color color;          ///< Color of the platform.
    float roundness;      ///< Roundness of the platform corners.
} Platform_st;


#endif // USER_TYPES_H