/**
 * @file suika.h
 * @brief Main header for Suika (Watermelon Game)
 * @author Multi Mini-Games Team
 * @date February 2026
 */

#ifndef SUIKA_SUIKA_H
#define SUIKA_SUIKA_H

#include <raylib.h>
#include <raymath.h>
#include <stdbool.h>
#include <stdint.h>

// Game constants
#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 900
#define TARGET_FPS 60

#define MAX_FRUITS 128
#define CONTAINER_WIDTH 600
#define CONTAINER_HEIGHT 700
#define CONTAINER_X 100
#define CONTAINER_Y 150
#define DROP_LINE_Y 200

// Fruit types (ordered by size)
typedef enum {
    FRUIT_CHERRY = 0,    // Smallest
    FRUIT_STRAWBERRY,
    FRUIT_GRAPE,
    FRUIT_ORANGE,
    FRUIT_PERSIMMON,
    FRUIT_APPLE,
    FRUIT_PEAR,
    FRUIT_PEACH,
    FRUIT_PINEAPPLE,
    FRUIT_MELON,
    FRUIT_WATERMELON,    // Largest
    FRUIT_TYPE_COUNT
} FruitType_Et;

// Fruit properties
typedef struct {
    float radius;
    Color color;
    int points;
    const char* emoji;
} FruitProperties_St;

// Individual fruit
typedef struct {
    Vector2 position;
    Vector2 velocity;
    FruitType_Et type;
    float radius;
    bool isActive;
    bool isMerging;
    int id;
} Fruit_St;

// Game state
typedef struct {
    Fruit_St fruits[MAX_FRUITS];
    int nextFruitId;
    Fruit_St nextFruit;
    float nextFruitX;
    bool canDrop;
    int score;
    int highScore;
    bool isGameOver;
    float gameOverTimer;
    float gravity;
} SuikaGame_St;

// Function declarations

/** @brief Initialize suika game */
void suika_init(SuikaGame_St* game);

/** @brief Update game logic */
void suika_update(SuikaGame_St* game, float deltaTime);

/** @brief Draw game */
void suika_draw(const SuikaGame_St* game);

/** @brief Cleanup resources */
void suika_cleanup(SuikaGame_St* game);

/** @brief Get fruit properties by type */
const FruitProperties_St* suika_getFruitProperties(FruitType_Et type);

/** @brief Spawn new fruit to drop */
void suika_spawnNextFruit(SuikaGame_St* game);

/** @brief Drop current fruit */
void suika_dropFruit(SuikaGame_St* game);

/** @brief Update physics for all fruits */
void suika_updatePhysics(SuikaGame_St* game, float deltaTime);

/** @brief Check and handle fruit merging */
void suika_checkMerging(SuikaGame_St* game);

/** @brief Check game over condition */
void suika_checkGameOver(SuikaGame_St* game);

/** @brief Reset game */
void suika_reset(SuikaGame_St* game);

/** @brief Draw HUD */
void suika_drawHUD(const SuikaGame_St* game);

#endif // SUIKA_SUIKA_H
