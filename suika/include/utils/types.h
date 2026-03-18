/**
    @file utils/types.h
    @author Multi Mini-Games Team
    @date February 2026
    @brief Type definitions for the Suika (Watermelon Game) mini-game.

    Defines all fruit types, fruit properties, game structures and constants
    used throughout the Suika game implementation.
*/

#ifndef SUIKA_UTILS_TYPES_H
#define SUIKA_UTILS_TYPES_H

#include "common.h"
#include "APIs/generalAPI.h"
#include "suika_atlas.h"

/**
    @brief Enumeration of all fruit types in the Suika game.

    Fruits are ordered by size from smallest (Cherry) to largest (Watermelon).
    When two fruits of the same type collide, they merge into the next larger type.
*/
typedef enum
{
    FRUIT_CHERRY = 0,       /**< Smallest fruit - radius 20px, 10 points */
    FRUIT_GRAPE,            /**< Second smallest - radius 25px, 30 points */
    FRUIT_STRAWBERRY,       /**< radius 30px, 20 points */
    FRUIT_CLEMENTINE,       /**< radius 35px, 50 points */
    FRUIT_ORANGE,           /**< radius 40px, 40 points */
    FRUIT_APPLE,            /**< radius 45px, 60 points */
    FRUIT_PEAR,             /**< radius 50px, 70 points */
    FRUIT_PEACH,            /**< radius 55px, 80 points */
    FRUIT_PINEAPPLE,        /**< radius 60px, 90 points */
    FRUIT_MELON,            /**< radius 70px, 100 points */
    FRUIT_WATERMELON,       /**< Largest fruit - radius 80px, 150 points */
    FRUIT_TYPE_COUNT        /**< Total number of fruit types */
} FruitType_Et;

/**
    @brief Sprite rectangle information for fruit rendering.

    Used internally to map fruit types to their sprite atlas positions.
*/
typedef struct
{
    const char* name;       /**< Fruit name identifier */
    int x;                  /**< X position in sprite atlas */
    int y;                  /**< Y position in sprite atlas */
    int w;                  /**< Width in sprite atlas */
    int h;                  /**< Height in sprite atlas */
} FruitSprite_St;

/**
    @brief Physical and visual properties for a fruit type.

    Contains the radius, color, point value, and sprite rectangle
    for rendering a specific fruit type.
*/
typedef struct
{
    float radius;           /**< Collision radius in pixels */
    Color color;            /**< Fallback color if sprite unavailable */
    int points;             /**< Points awarded when this fruit is created */
    Rectangle spriteRect;   /**< Source rectangle in the fruit atlas */
} FruitProperties_St;

/**
    @brief Runtime state for a single fruit instance.

    Tracks position, velocity, and state for physics simulation
    and rendering of an individual fruit.
*/
typedef struct
{
    Vector2 position;       /**< Current position in world coordinates */
    Vector2 velocity;       /**< Current velocity in pixels/second */
    FruitType_Et type;      /**< Fruit type determining size and merge behavior */
    float radius;           /**< Current collision radius */
    float rotation;         /**< Current rotation angle in radians */
    float angularVelocity;  /**< Rotation speed in radians/second */
    bool isActive;          /**< Whether this fruit is currently in play */
    bool isMerging;         /**< Temporary flag during merge animation */
    int id;                 /**< Unique identifier for this fruit instance */
    float glowIntensity;   /**< Glow effect intensity for merge animation */
} Fruit_St;

/**
    @brief Particle for visual effects (merges, impacts).
*/
typedef struct
{
    Vector2 position;
    Vector2 velocity;
    Color color;
    float life;
    float maxLife;
    float size;
    bool isActive;
} Particle_St;

#define SUIKA_MAX_PARTICLES 64

/**
    @brief Main game state structure for Suika.

    Contains all state needed to run the Suika game including
    the fruit pool, score, and game configuration.

    @note Must have BaseGame_St as first member for lobby compatibility.
*/
struct SuikaGame_St
{
    BaseGame_St base;               /**< Base game interface for lobby integration */

    Texture2D fruitAtlas;           /**< Sprite atlas containing all fruit images */

    Fruit_St fruits[SUIKA_MAX_FRUITS];  /**< Pool of all fruit instances */
    int nextFruitId;                /**< Counter for unique fruit IDs */
    Fruit_St nextFruit;             /**< Preview of next fruit to drop */
    float nextFruitX;               /**< X position for next fruit drop */
    bool canDrop;                   /**< Whether player can drop a new fruit */
    float dropTimer;                /**< Time since last drop (for cooldown) */
    long score;                     /**< Current game score */
    long highScore;                 /**< Best score this session */
    bool isGameOver;                /**< Whether game has ended */
    float gameOverTimer;            /**< Time since game over */
    float gravity;                  /**< Gravity acceleration in pixels/second^2 */
    
    // Fonctionnalité capacité 'P' - Auto-drop avec score désactivé
    bool autoDropEnabled;           /**< Mode dépôt automatique activé (touche P) */
    bool scoreMultiplierEnabled;    /**< Multiplicateur de score actif (désactivé pendant auto-drop) */
    float boostCooldown;            /**< Temps de recharge du boost */
    float baseDropCooldown;         /**< Temps de base entre les dépôts (1 seconde) */
    
    // Système de particules pour les effets visuels
    Particle_St particles[SUIKA_MAX_PARTICLES];
    int particleCount;
};

typedef struct SuikaGame_St SuikaGame_St;

#endif
