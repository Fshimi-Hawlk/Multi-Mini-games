/**
 * @file game.c
 * @brief Core Suika game logic with sprite atlas support
 * @author Multi Mini-Games Team
 * @date February 2026
 */

#include "core/game.h"
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include "raymath.h"
#include "logger.h"

static char g_assetPath[512] = {0};

static const char* suika_getAssetPath(void)
{
    if (g_assetPath[0] != '\0')
        return g_assetPath;
    
    const char* relativePaths[] = {
        "assets/", "suika/assets/", "../suika/assets/",
        "../../assets/", "../../suika/assets/", "../assets/"
    };
    
    const char* baseDirs[] = {GetWorkingDirectory(), GetApplicationDirectory()};
    
    for (int d = 0; d < 2; d++)
    {
        if (!baseDirs[d] || baseDirs[d][0] == '\0')
            continue;
        for (int i = 0; i < (int)(sizeof(relativePaths)/sizeof(relativePaths[0])); i++)
        {
            char testPath[512];
            snprintf(testPath, sizeof(testPath), "%s%s%sassets_suika.png", 
                     baseDirs[d], d == 0 ? "/" : "", relativePaths[i]);
            if (FileExists(testPath))
            {
                snprintf(g_assetPath, sizeof(g_assetPath), "%s%s%s", 
                         baseDirs[d], d == 0 ? "/" : "", relativePaths[i]);
                return g_assetPath;
            }
        }
    }
    
    strcpy(g_assetPath, "assets/");
    return g_assetPath;
}

static Texture2D suika_loadTexture(const char* path)
{
    Texture2D tex = LoadTexture(path);
    if (tex.id == 0)
    {
        log_warn("Failed to load texture: %s", path);
    }
    return tex;
}

static const FruitProperties_St FRUIT_PROPS[FRUIT_TYPE_COUNT] = {
    // {radius,  color,           points,  {x,   y,   w,   h}}
    {22.5f,  {255, 0, 0, 255},       10,   {44,  130,  45,  70}},   // cherry
    {22.0f,  {128, 0, 128, 255},     30,   {116, 136,  44,  64}},   // grape
    {20.0f,  {255, 100, 100, 255},   20,   {216, 143,  40,  57}},   // strawberry
    {48.0f,  {255, 165, 0, 255},      50,   {304, 109,  96, 113}},   // small_orange
    {56.5f,  {255, 140, 0, 255},     40,   {451, 114, 113, 117}},   // big_orange
    {57.0f,  {255, 0, 0, 255},       60,   {579,  88, 114, 134}},   // apple
    {51.0f,  {173, 255, 47, 255},    70,   {58,  277, 102, 146}},   // pear
    {56.5f,  {255, 218, 185, 255},   80,   {210, 305, 123, 113}},   // peach
    {53.0f,  {255, 215, 0, 255},      90,   {369, 248, 106, 194}},   // pineapple
    {61.0f,  {144, 238, 144, 255},   100,   {510, 288, 122, 144}},   // melon
    {84.0f,  {34, 139, 34, 255},     150,   {672, 272, 168, 200}}   // watermelon
};

const FruitProperties_St* suika_getFruitProperties(FruitType_Et type)
{
    if (type >= 0 && type < FRUIT_TYPE_COUNT)
    {
        return &FRUIT_PROPS[type];
    }
    return &FRUIT_PROPS[0];
}

void suika_loadAssets(SuikaGame_St* game)
{
    const char* assetPath = suika_getAssetPath();
    char fullPath[512];
    snprintf(fullPath, sizeof(fullPath), "%sassets_suika.png", assetPath);
    game->fruitAtlas = suika_loadTexture(fullPath);
}

void suika_unloadAssets(SuikaGame_St* game)
{
    if (game->fruitAtlas.id != 0)
    {
        UnloadTexture(game->fruitAtlas);
        game->fruitAtlas.id = 0;
    }
}

void suika_init(SuikaGame_St* game)
{
    memset(game->fruits, 0, sizeof(game->fruits));

    game->nextFruitId = 0;
    game->nextFruitX = SUIKA_CONTAINER_X + SUIKA_CONTAINER_WIDTH / 2.0f;
    game->canDrop = true;
    game->dropTimer = 0.0f;
    game->score = 0;
    game->highScore = 0;
    game->isGameOver = false;
    game->gravity = 800.0f;

    // Initialisation des nouvelles fonctionnalités
    game->autoDropEnabled = false;
    game->scoreMultiplierEnabled = true;
    game->boostCooldown = 0.0f;
    game->baseDropCooldown = 1.0f;

    for (int i = 0; i < SUIKA_MAX_FRUITS; i++)
    {
        game->fruits[i].isActive = false;
        game->fruits[i].isMerging = false;
        game->fruits[i].rotation = 0.0f;
        game->fruits[i].angularVelocity = 0.0f;
    }

    suika_spawnNextFruit(game);
}

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
            game->fruits[i].position.x = game->nextFruitX;

            game->canDrop = false;
            suika_spawnNextFruit(game);
            break;
        }
    }
}

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

    // Gestion de la touche P pour activer/désactiver le mode auto-drop
    if (IsKeyPressed(KEY_P))
    {
        game->autoDropEnabled = !game->autoDropEnabled;
        game->scoreMultiplierEnabled = !game->autoDropEnabled; // Désactive le score en mode auto-drop
    }

    // Mise à jour du cooldown du boost
    if (game->boostCooldown > 0.0f)
    {
        game->boostCooldown -= deltaTime;
    }

    Vector2 mousePos = GetMousePosition();
    float minX = SUIKA_CONTAINER_X + game->nextFruit.radius;
    float maxX = SUIKA_CONTAINER_X + SUIKA_CONTAINER_WIDTH - game->nextFruit.radius;
    game->nextFruitX = Clamp(mousePos.x, minX, maxX);
    game->nextFruit.position.x = game->nextFruitX;

    // Mode auto-drop : spawn automatique des fruits
    if (game->autoDropEnabled && game->canDrop)
    {
        suika_dropFruit(game);
    }
    // Mode normal : clic pour drop
    else if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
    {
        suika_dropFruit(game);
    }

    suika_updatePhysics(game, deltaTime);
    suika_checkMerging(game);
    suika_checkGameOver(game);

    if (!game->canDrop)
    {
        game->dropTimer += deltaTime;
        // En mode auto-drop, le délai est réduit par 4
        float currentCooldown = game->autoDropEnabled ? game->baseDropCooldown / 4.0f : game->baseDropCooldown;
        if (game->dropTimer > currentCooldown)
        {
            game->canDrop = true;
            game->dropTimer = 0.0f;
        }
    }
}

void suika_updatePhysics(SuikaGame_St* game, float deltaTime)
{
    const float linearDamping = 0.99f;
    const float angularDamping = 0.95f;
    const float groundFriction = 0.9f;
    const float restitution = 0.1f;
    const float wallRestitution = 0.1f;
    const float terminalVelocity = 800.0f;
    
    for (int i = 0; i < SUIKA_MAX_FRUITS; i++)
    {
        if (!game->fruits[i].isActive || game->fruits[i].isMerging)
            continue;

        Fruit_St* fruit = &game->fruits[i];

        fruit->velocity.y += game->gravity * deltaTime;
        
        if (fruit->velocity.y > terminalVelocity)
        {
            fruit->velocity.y = terminalVelocity;
        }
        if (fruit->velocity.y < -terminalVelocity)
        {
            fruit->velocity.y = -terminalVelocity;
        }
        
        fruit->velocity.x *= linearDamping;
        fruit->velocity.y *= linearDamping;
        fruit->angularVelocity *= angularDamping;

        fruit->position.x += fruit->velocity.x * deltaTime;
        fruit->position.y += fruit->velocity.y * deltaTime;
        
        fruit->rotation += fruit->angularVelocity * deltaTime;

        float minX = SUIKA_CONTAINER_X + fruit->radius;
        float maxX = SUIKA_CONTAINER_X + SUIKA_CONTAINER_WIDTH - fruit->radius;
        float maxY = SUIKA_CONTAINER_Y + SUIKA_CONTAINER_HEIGHT - fruit->radius;

        if (fruit->position.x < minX)
        {
            fruit->position.x = minX;
            fruit->velocity.x = -fruit->velocity.x * wallRestitution;
            fruit->angularVelocity += fruit->velocity.y * 0.03f;
        }
        else if (fruit->position.x > maxX)
        {
            fruit->position.x = maxX;
            fruit->velocity.x = -fruit->velocity.x * wallRestitution;
            fruit->angularVelocity -= fruit->velocity.y * 0.03f;
        }

        if (fruit->position.y > maxY)
        {
            fruit->position.y = maxY;
            fruit->velocity.y = -fruit->velocity.y * restitution;
            if (fabsf(fruit->velocity.y) < 20.0f)
            {
                fruit->velocity.y = 0.0f;
            }
            fruit->velocity.x *= groundFriction;
            fruit->angularVelocity = fruit->velocity.x * 0.02f;
        }

        for (int j = i + 1; j < SUIKA_MAX_FRUITS; j++)
        {
            if (!game->fruits[j].isActive || game->fruits[j].isMerging)
                continue;

            Fruit_St* other = &game->fruits[j];
            float dist = Vector2Distance(fruit->position, other->position);
            float minDist = fruit->radius + other->radius;

            if (dist < minDist && dist > 0.001f)
            {
                Vector2 normal = Vector2Normalize(Vector2Subtract(other->position, fruit->position));
                float overlap = minDist - dist;

                float totalMass = fruit->radius + other->radius;
                float ratio1 = other->radius / totalMass;
                float ratio2 = fruit->radius / totalMass;

                fruit->position = Vector2Subtract(fruit->position, Vector2Scale(normal, overlap * ratio1));
                other->position = Vector2Add(other->position, Vector2Scale(normal, overlap * ratio2));

                Vector2 relativeVel = Vector2Subtract(fruit->velocity, other->velocity);
                float velAlongNormal = Vector2DotProduct(relativeVel, normal);

                if (velAlongNormal < 0)
                {
                    float fruitMass = fruit->radius * fruit->radius;
                    float otherMass = other->radius * other->radius;
                    float totalInvMass = 1.0f / fruitMass + 1.0f / otherMass;
                    
                    float restitutionFactor = 0.5f;
                    float j = -(1.0f + restitutionFactor) * velAlongNormal / totalInvMass;
                    Vector2 impulse = Vector2Scale(normal, j);
                    
                    fruit->velocity = Vector2Subtract(fruit->velocity, Vector2Scale(impulse, 1.0f / fruitMass));
                    other->velocity = Vector2Add(other->velocity, Vector2Scale(impulse, 1.0f / otherMass));
                    
                    Vector2 tangent = Vector2Subtract(relativeVel, Vector2Scale(normal, velAlongNormal));
                    float tangentLen = Vector2Length(tangent);
                    if (tangentLen > 0.001f)
                    {
                        tangent = Vector2Scale(tangent, 1.0f / tangentLen);
                        float frictionImpulse = tangentLen * 0.3f / totalInvMass;
                        fruit->velocity = Vector2Subtract(fruit->velocity, Vector2Scale(tangent, frictionImpulse / fruitMass));
                        other->velocity = Vector2Add(other->velocity, Vector2Scale(tangent, frictionImpulse / otherMass));
                    }
                    
                    float angularImpulse = velAlongNormal * 0.05f;
                    fruit->angularVelocity += angularImpulse;
                    other->angularVelocity -= angularImpulse;
                }
            }
        }
    }
}

void suika_checkMerging(SuikaGame_St* game)
{
    for (int i = 0; i < SUIKA_MAX_FRUITS; i++)
    {
        if (!game->fruits[i].isActive || game->fruits[i].isMerging)
            continue;

        for (int j = i + 1; j < SUIKA_MAX_FRUITS; j++)
        {
            if (!game->fruits[j].isActive || game->fruits[j].isMerging)
                continue;

            Fruit_St* f1 = &game->fruits[i];
            Fruit_St* f2 = &game->fruits[j];

            if (f1->type == f2->type)
            {
                float dist = Vector2Distance(f1->position, f2->position);
                float touchDist = f1->radius + f2->radius;

                if (dist < touchDist * 1.1f)
                {
                    if (f1->type < FRUIT_WATERMELON)
                    {
                        FruitType_Et newType = (FruitType_Et)(f1->type + 1);
                        const FruitProperties_St* props = suika_getFruitProperties(newType);

                        for (int k = 0; k < SUIKA_MAX_FRUITS; k++)
                        {
                            if (!game->fruits[k].isActive)
                            {
                                Vector2 midPos = Vector2Scale(Vector2Add(f1->position, f2->position), 0.5f);

                                game->fruits[k].position = midPos;
                                game->fruits[k].velocity = (Vector2){0.0f, 0.0f};
                                game->fruits[k].type = newType;
                                game->fruits[k].radius = props->radius;
                                game->fruits[k].rotation = (f1->rotation + f2->rotation) * 0.5f;
                                game->fruits[k].angularVelocity = (f1->angularVelocity + f2->angularVelocity) * 0.5f;
                                game->fruits[k].isActive = true;
                                game->fruits[k].isMerging = false;
                                game->fruits[k].id = game->nextFruitId++;

                                // Ajouter le score seulement si le multiplicateur est activé
                                if (game->scoreMultiplierEnabled)
                                {
                                    game->score += props->points;
                                }

                                f1->isActive = false;
                                f2->isActive = false;

                                return;
                            }
                        }
                    }
                }
            }
        }
    }
}

void suika_checkGameOver(SuikaGame_St* game)
{
    for (int i = 0; i < SUIKA_MAX_FRUITS; i++)
    {
        if (game->fruits[i].isActive)
        {
            if (game->fruits[i].position.y - game->fruits[i].radius < SUIKA_DROP_LINE_Y)
            {
                if (fabsf(game->fruits[i].velocity.y) < 10.0f)
                {
                    game->isGameOver = true;
                    if (game->score > game->highScore)
                    {
                        game->highScore = game->score;
                    }
                    game->base.running = false;
                }
            }
        }
    }
}

void suika_reset(SuikaGame_St* game)
{
    game->score = 0;
    game->isGameOver = false;
    game->gameOverTimer = 0.0f;
    game->canDrop = true;
    game->dropTimer = 0.0f;
    game->nextFruitId = 0;
    game->base.running = true;

    for (int i = 0; i < SUIKA_MAX_FRUITS; i++)
    {
        game->fruits[i].isActive = false;
        game->fruits[i].isMerging = false;
        game->fruits[i].rotation = 0.0f;
        game->fruits[i].angularVelocity = 0.0f;
    }

    suika_spawnNextFruit(game);
}

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

static void suika_drawGradientBackground(void)
{
    for (int y = 0; y < SUIKA_SCREEN_HEIGHT; y++)
    {
        float t = (float)y / SUIKA_SCREEN_HEIGHT;
        Color top = (Color){25, 25, 50, 255};
        Color bottom = (Color){60, 40, 80, 255};
        Color c = {
            (unsigned char)(top.r + (bottom.r - top.r) * t),
            (unsigned char)(top.g + (bottom.g - top.g) * t),
            (unsigned char)(top.b + (bottom.b - top.b) * t),
            255
        };
        DrawLine(0, y, SUIKA_SCREEN_WIDTH, y, c);
    }
}

static void suika_drawContainer(void)
{
    Rectangle container = {SUIKA_CONTAINER_X, SUIKA_CONTAINER_Y, 
                           SUIKA_CONTAINER_WIDTH, SUIKA_CONTAINER_HEIGHT};
    
    Color bgColor = (Color){40, 40, 60, 200};
    DrawRectangleRec(container, bgColor);
    
    Color borderColor = (Color){100, 100, 140, 255};
    DrawRectangleLinesEx(container, 3.0f, borderColor);
    
    Color dropLineColor = (Color){255, 100, 100, 150};
    DrawLine(SUIKA_CONTAINER_X + 3, SUIKA_DROP_LINE_Y, 
             SUIKA_CONTAINER_X + SUIKA_CONTAINER_WIDTH - 3, SUIKA_DROP_LINE_Y, 
             dropLineColor);
}

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

    suika_drawHUD(game);
}

static void suika_drawPanel(int x, int y, int width, int height, Color bgColor)
{
    DrawRectangle(x, y, width, height, bgColor);
    DrawRectangleLinesEx((Rectangle){x, y, width, height}, 2.0f, (Color){150, 150, 180, 255});
}

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

void suika_cleanup(SuikaGame_St* game)
{
    (void)game;
}


