/**
    @file game.c
    @author Maxime CHAUVEAU
    @date 2026-04-14
    @date 2026-04-14
    @brief Core Suika game logic - rendering and state management.
*/
#include "core/game.h"
#include "core/physics.h"
#include "utils/audio.h"
#include "assetPath.h"
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "raymath.h"
#include "logger.h"

/**
    @brief Resolves the base path for Suika assets.

    @param[in,out] game Pointer to the game state
    @return             Pointer to the resolved asset path string
*/
static const char* suika_resolveAssetPath(SuikaGame_St* game)
{
    if (game->assetPath[0] != '\0')
        return game->assetPath;

    static const char* candidates[] = {
        "jeux/suika/assets/",
        "assets/",
        "suika/assets/",
        "../suika/assets/",
        NULL
    };

    findAssetBase("assets_suika.png", candidates,
                  game->assetPath, sizeof(game->assetPath));
    return game->assetPath;
}

/**
    @brief Loads a texture from the given path.

    @param[in]     path Path to the texture file
    @return             Loaded Texture2D object
*/
static Texture2D suika_loadTexture(const char* path)
{
    Texture2D tex = LoadTexture(path);
    if (tex.id == 0)
    {
        log_warn("Failed to load texture: %s", path);
    }
    return tex;
}

/**
    @brief Static array containing physical and visual properties for each fruit type.
*/
static const FruitProperties_St FRUIT_PROPS[FRUIT_TYPE_COUNT] = {
    {15.0f,  {220,  50,  50, 255},    10,   { 44, 130,  45,  70}},
    {19.0f,  {128,   0, 128, 255},    30,   {116, 136,  44,  64}},
    {24.0f,  {255, 100, 100, 255},    20,   {216, 143,  40,  57}},
    {31.0f,  {255, 165,   0, 255},    50,   {304, 109,  96, 113}},
    {38.0f,  {255, 120,   0, 255},    40,   {451, 114, 113, 117}},
    {44.0f,  {220,  30,  30, 255},    60,   {579,  88, 114, 134}},
    {50.0f,  {150, 210,  50, 255},    70,   { 58, 277, 102, 146}},
    {57.0f,  {255, 180, 130, 255},    80,   {210, 305, 123, 113}},
    {64.0f,  {255, 200,  40, 255},    90,   {369, 248, 106, 194}},
    {72.0f,  {100, 210, 100, 255},   100,   {510, 288, 122, 144}},
    {84.0f,  { 34, 139,  34, 255},   150,   {672, 272, 168, 200}}
};

/**
    @brief Gets the properties for a specific fruit type.

    @param[in]     type The fruit type
    @return             Pointer to the FruitProperties_St for the given type
*/
const FruitProperties_St* suika_getFruitProperties(FruitType_Et type)
{
    if (type >= 0 && type < FRUIT_TYPE_COUNT)
    {
        return &FRUIT_PROPS[type];
    }
    return &FRUIT_PROPS[0];
}

/**
    @brief Updates all active particles.

    @param[in,out] game      Pointer to the game state
    @param[in]     deltaTime Time elapsed since last frame
*/
static void suika_updateParticles(SuikaGame_St* game, float deltaTime)
{
    for (int i = 0; i < game->particleCount; i++)
    {
        Particle_St* p = &game->particles[i];
        if (!p->isActive) continue;
        
        p->life -= deltaTime;
        if (p->life <= 0.0f)
        {
            p->isActive = false;
            continue;
        }
        
        p->velocity.y += 200.0f * deltaTime;
        p->position.x += p->velocity.x * deltaTime;
        p->position.y += p->velocity.y * deltaTime;
        p->velocity.x *= 0.98f;
        p->velocity.y *= 0.98f;
    }
    
    int write = 0;
    for (int read = 0; read < game->particleCount; read++)
    {
        if (game->particles[read].isActive)
        {
            if (write != read)
            {
                game->particles[write] = game->particles[read];
            }
            write++;
        }
    }
    game->particleCount = write;
}

/**
    @brief Draws all active particles.

    @param[in]     game Pointer to the game state
*/
static void suika_drawParticles(const SuikaGame_St* game)
{
    for (int i = 0; i < game->particleCount; i++)
    {
        const Particle_St* p = &game->particles[i];
        if (!p->isActive) continue;
        
        float alpha = p->life / p->maxLife;
        Color c = p->color;
        c.a = (unsigned char)(alpha * 255);
        
        DrawCircleV(p->position, p->size * alpha, c);
    }
}

/**
    @brief Loads all required textures for Suika.

    @param[in,out] game Pointer to the game state
*/
void suika_loadAssets(SuikaGame_St* game)
{
    const char* assetPath = suika_resolveAssetPath(game);
    char fullPath[512];
    snprintf(fullPath, sizeof(fullPath), "%sassets_suika.png", assetPath);
    game->fruitAtlas = suika_loadTexture(fullPath);
}

/**
    @brief Unloads all Suika assets.

    @param[in,out] game Pointer to the game state
*/
void suika_unloadAssets(SuikaGame_St* game)
{
    if (game->fruitAtlas.id != 0)
    {
        UnloadTexture(game->fruitAtlas);
        game->fruitAtlas.id = 0;
    }
}

/**
    @brief Initializes the game state.

    @param[in,out] game Pointer to the game state
*/
void suika_init(SuikaGame_St* game)
{
    memset(game->fruits, 0, sizeof(game->fruits));
    memset(game->particles, 0, sizeof(game->particles));
    game->particleCount = 0;

    game->nextFruitId = 0;
    game->nextFruitX = SUIKA_CONTAINER_X + SUIKA_CONTAINER_WIDTH / 2.0f;
    game->canDrop = true;
    game->dropTimer = 0.0f;
    game->score = 0;
    game->highScore = 0;
    game->isGameOver = false;
    game->gravity = 800.0f;

    game->autoDropEnabled = false;
    game->scoreMultiplierEnabled = true;
    game->baseDropCooldown = 1.0f;

    for (int i = 0; i < SUIKA_MAX_FRUITS; i++)
    {
        game->fruits[i].isActive = false;
        game->fruits[i].isMerging = false;
        game->fruits[i].rotation = 0.0f;
        game->fruits[i].angularVelocity = 0.0f;
        game->fruits[i].glowIntensity = 0.0f;
    }

    suika_spawnNextFruit(game);
}

/**
    @brief Spawns a new preview fruit.

    @param[in,out] game Pointer to the game state
*/
void suika_spawnNextFruit(SuikaGame_St* game)
{
    FruitType_Et type = (FruitType_Et)(rand() % 5);
    const FruitProperties_St* props = suika_getFruitProperties(type);

    game->nextFruit.type = type;
    game->nextFruit.radius = props->radius;
    game->nextFruit.position = (Vector2){game->nextFruitX, SUIKA_DROP_LINE_Y - props->radius};
    game->nextFruit.velocity = (Vector2){0.0f, 0.0f};
    game->nextFruit.rotation = 0.0f;
    game->nextFruit.angularVelocity = 0.0f;
    game->nextFruit.isActive = false;
    game->nextFruit.isMerging = false;
    game->nextFruit.id = game->nextFruitId++;
}

/**
    @brief Drops the current preview fruit into the container.

    @param[in,out] game Pointer to the game state
*/
void suika_dropFruit(SuikaGame_St* game)
{
    if (!game->canDrop || game->isGameOver)
        return;

    for (int i = 0; i < SUIKA_MAX_FRUITS; i++)
    {
        if (!game->fruits[i].isActive)
        {
            game->fruits[i] = game->nextFruit;
            game->fruits[i].isActive = true;
            game->fruits[i].position.x = game->nextFruitX + (float)(rand() % 7 - 3);

            game->canDrop = false;
            suika_spawnNextFruit(game);
            break;
        }
    }

}

/**
    @brief Main update function for game logic.

    @param[in,out] game      Pointer to the game state
    @param[in]     deltaTime Time elapsed since last frame
*/
void suika_update(SuikaGame_St* game, float deltaTime)
{
    if (game->isGameOver)
    {
        game->gameOverTimer += deltaTime;

        if (IsKeyPressed(KEY_R))
        {
            suika_reset(game);
        }
        return;
    }

    if (IsKeyPressed(KEY_P))
    {
        game->autoDropEnabled = !game->autoDropEnabled;
        game->scoreMultiplierEnabled = !game->autoDropEnabled;
    }

    Vector2 mousePos = GetMousePosition();
    float minX = SUIKA_CONTAINER_X + game->nextFruit.radius;
    float maxX = SUIKA_CONTAINER_X + SUIKA_CONTAINER_WIDTH - game->nextFruit.radius;
    game->nextFruitX = Clamp(mousePos.x, minX, maxX);
    game->nextFruit.position.x = game->nextFruitX;

    if (game->autoDropEnabled && game->canDrop)
    {
        suika_dropFruit(game);
    }
    else if (!game->autoDropEnabled && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
    {
        suika_dropFruit(game);
        PlaySound(sound_drop);
    }

    suika_updatePhysics(game, deltaTime);
    suika_checkMerging(game);
    suika_checkGameOver(game);
    
    suika_updateParticles(game, deltaTime);

    if (!game->canDrop)
    {
        game->dropTimer += deltaTime;
        float currentCooldown = game->autoDropEnabled ? 0.8f : game->baseDropCooldown;
        if (game->dropTimer > currentCooldown)
        {
            game->canDrop = true;
            game->dropTimer = 0.0f;
        }
    }
}

/**
    @brief Resets the game to its initial state.

    @param[in,out] game Pointer to the game state
*/
void suika_reset(SuikaGame_St* game)
{
    game->score = 0;
    game->isGameOver = false;
    game->gameOverTimer = 0.0f;
    game->canDrop = true;
    game->dropTimer = 0.0f;
    game->nextFruitId = 0;
    game->base.running = true;
    game->particleCount = 0;

    for (int i = 0; i < SUIKA_MAX_FRUITS; i++)
    {
        game->fruits[i].isActive = false;
        game->fruits[i].isMerging = false;
        game->fruits[i].rotation = 0.0f;
        game->fruits[i].angularVelocity = 0.0f;
    }

    suika_spawnNextFruit(game);
}

/**
    @brief Helper function to draw a single fruit.

    @param[in]     game  Pointer to the game state
    @param[in]     fruit Pointer to the fruit instance to draw
    @param[in]     alpha Opacity of the fruit
*/
static void suika_drawFruit(const SuikaGame_St* game, const Fruit_St* fruit, float alpha)
{
    const FruitProperties_St* props = suika_getFruitProperties(fruit->type);

    if (game->fruitAtlas.id == 0)
    {
        return;
    }

    Color shadowColor = {0, 0, 0, (unsigned char)(40 * alpha)};
    Vector2 shadowPos = {fruit->position.x + 3, fruit->position.y + 5};
    DrawCircleV(shadowPos, fruit->radius * 0.9f, shadowColor);

    Rectangle src = props->spriteRect;
    Rectangle dest = {
        fruit->position.x,
        fruit->position.y,
        fruit->radius * 2.0f,
        fruit->radius * 2.0f
    };
    Vector2 origin = {fruit->radius, fruit->radius};

    Color tint = WHITE;
    if (alpha < 1.0f)
    {
        tint.a = (unsigned char)(alpha * 255);
    }

    DrawTexturePro(game->fruitAtlas, src, dest, origin, fruit->rotation * RAD2DEG, tint);
}

/**
    @brief Draws the gradient background.
*/
static void suika_drawGradientBackground(void)
{
    Color topColor = (Color){20, 20, 40, 255};
    Color bottomColor = (Color){50, 30, 70, 255};
    
    Rectangle topRect = {0, 0, SUIKA_SCREEN_WIDTH, SUIKA_SCREEN_HEIGHT / 2};
    Rectangle bottomRect = {0, SUIKA_SCREEN_HEIGHT / 2, SUIKA_SCREEN_WIDTH, SUIKA_SCREEN_HEIGHT / 2};
    
    DrawRectangleRec(topRect, topColor);
    DrawRectangleRec(bottomRect, bottomColor);
}

/**
    @brief Draws the container and boundaries.
*/
static void suika_drawContainer(void)
{
    Rectangle container = {SUIKA_CONTAINER_X, SUIKA_CONTAINER_Y, 
                           SUIKA_CONTAINER_WIDTH, SUIKA_CONTAINER_HEIGHT};
    
    Color bgColor = (Color){35, 35, 55, 230};
    DrawRectangleRec(container, bgColor);
    
    Color innerBorder = (Color){50, 50, 70, 255};
    DrawRectangleLinesEx(container, 2.0f, innerBorder);
    
    Color outerBorder = (Color){120, 120, 160, 255};
    Rectangle outerRect = {container.x - 2, container.y - 2, container.width + 4, container.height + 4};
    DrawRectangleLinesEx(outerRect, 4.0f, outerBorder);
    
    Color dropLineColor = (Color){255, 120, 120, 180};
    DrawLine(SUIKA_CONTAINER_X + 5, SUIKA_DROP_LINE_Y, 
             SUIKA_CONTAINER_X + SUIKA_CONTAINER_WIDTH - 5, SUIKA_DROP_LINE_Y, 
             dropLineColor);
}

/**
    @brief Main draw function.

    @param[in]     game Pointer to the game state
*/
void suika_draw(const SuikaGame_St* game)
{
    suika_drawGradientBackground();
    suika_drawContainer();

    for (int i = 0; i < SUIKA_MAX_FRUITS; i++)
    {
        if (game->fruits[i].isActive)
        {
            suika_drawFruit(game, &game->fruits[i], 1.0f);
        }
    }

    if (game->canDrop && !game->isGameOver)
    {
        suika_drawFruit(game, &game->nextFruit, 0.5f);
    }
    
    suika_drawParticles(game);
    
    suika_drawHUD(game);
}

/**
    @brief Draws a styled panel.

    @param[in]     x       X coordinate
    @param[in]     y       Y coordinate
    @param[in]     width   Width of the panel
    @param[in]     height  Height of the panel
    @param[in]     bgColor Background color
*/
static void suika_drawPanel(int x, int y, int width, int height, Color bgColor)
{
    DrawRectangle(x, y, width, height, bgColor);
    DrawRectangleLinesEx((Rectangle){x, y, width, height}, 2.0f, (Color){150, 150, 180, 255});
}

/**
    @brief Draws the Head-Up Display (HUD).

    @param[in]     game Pointer to the game state
*/
void suika_drawHUD(const SuikaGame_St* game)
{
    suika_drawPanel(10, 10, 200, 100, (Color){30, 30, 50, 200});
    
    DrawText("SUIKA", 70, 18, 28, (Color){255, 220, 100, 255});
    DrawText(TextFormat("Score: %ld", game->score), 20, 50, 18, (Color){200, 200, 255, 255});
    DrawText(TextFormat("Best: %ld", game->highScore), 20, 75, 16, (Color){150, 255, 150, 255});

    if (game->isGameOver)
    {
        DrawRectangle(0, SUIKA_SCREEN_HEIGHT/2 - 60, SUIKA_SCREEN_WIDTH, 120, Fade(BLACK, 0.8f));
        
        Color gameOverColor = (Color){255, 80, 80, 255};
        DrawText("GAME OVER!", SUIKA_SCREEN_WIDTH/2 - 120, SUIKA_SCREEN_HEIGHT/2 - 30, 45, gameOverColor);
        
        Color restartColor = (Color){200, 200, 200, 255};
        DrawText("Press R to restart", SUIKA_SCREEN_WIDTH/2 - 100, SUIKA_SCREEN_HEIGHT/2 + 25, 22, restartColor);
    }
    else
    {
        suika_drawPanel(10, SUIKA_SCREEN_HEIGHT - 75, 220, 60, (Color){30, 30, 50, 180});
        DrawText("Click to drop fruit", 20, SUIKA_SCREEN_HEIGHT - 65, 14, (Color){180, 180, 200, 255});
        DrawText("R: Restart | ESC: Quit", 20, SUIKA_SCREEN_HEIGHT - 45, 14, (Color){180, 180, 200, 255});
    }
}

/**
    @brief Cleanup function for Suika game state.

    @param[in,out] game Pointer to the game state
*/
void suika_cleanup(SuikaGame_St* game)
{
    (void)game;
}
