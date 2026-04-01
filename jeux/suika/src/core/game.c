/**
 * @file game.c
 * @brief Core Suika game logic with sprite atlas support
 * @author Multi Mini-Games Team
 * @date February 2026
 */

#include "core/game.h"
#include "assetPath.h"
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include "raymath.h"
#include "logger.h"
#include "audio.h"

/* OPT: asset path is now stored in game->assetPath (no static global → multiple instances safe) */
static const char* suika_resolveAssetPath(SuikaGame_St* game)
{
    if (game->assetPath[0] != '\0')
        return game->assetPath;

    static const char* candidates[] = {
        "jeux/suika/assets/",   /* default: from project root */
        "assets/",
        "suika/assets/",
        "../suika/assets/",
        NULL
    };

    findAssetBase("assets_suika.png", candidates,
                  game->assetPath, sizeof(game->assetPath));
    return game->assetPath;
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
    // {radius,  color,               points,  {x,   y,   w,   h}}
    {15.0f,  {220,  50,  50, 255},    10,   { 44, 130,  45,  70}},   // cherry
    {19.0f,  {128,   0, 128, 255},    30,   {116, 136,  44,  64}},   // grape
    {24.0f,  {255, 100, 100, 255},    20,   {216, 143,  40,  57}},   // strawberry
    {31.0f,  {255, 165,   0, 255},    50,   {304, 109,  96, 113}},   // clementine
    {38.0f,  {255, 120,   0, 255},    40,   {451, 114, 113, 117}},   // orange
    {44.0f,  {220,  30,  30, 255},    60,   {579,  88, 114, 134}},   // apple
    {50.0f,  {150, 210,  50, 255},    70,   { 58, 277, 102, 146}},   // pear
    {57.0f,  {255, 180, 130, 255},    80,   {210, 305, 123, 113}},   // peach
    {64.0f,  {255, 200,  40, 255},    90,   {369, 248, 106, 194}},   // pineapple
    {72.0f,  {100, 210, 100, 255},   100,   {510, 288, 122, 144}},   // melon
    {84.0f,  { 34, 139,  34, 255},   150,   {672, 272, 168, 200}}    // watermelon
};

const FruitProperties_St* suika_getFruitProperties(FruitType_Et type)
{
    if (type >= 0 && type < FRUIT_TYPE_COUNT)
    {
        return &FRUIT_PROPS[type];
    }
    return &FRUIT_PROPS[0];
}

// ============================================
// SYSTÈME DE PARTICULES POUR LES EFFETS VISUELS
// ============================================

static void suika_spawnMergeParticles(SuikaGame_St* game, Vector2 position, Color color)
{
    // Spawn 8-12 particules pour l'effet de fusion
    int count = 8 + (rand() % 5);
    for (int i = 0; i < count && game->particleCount < SUIKA_MAX_PARTICLES; i++)
    {
        Particle_St* p = &game->particles[game->particleCount++];
        float angle = (float)(rand() % 360) * DEG2RAD;
        float speed = 50.0f + (float)(rand() % 100);
        
        p->position = position;
        p->velocity = (Vector2){cosf(angle) * speed, sinf(angle) * speed - 50.0f};
        p->color = color;
        p->color.a = 255;
        p->life = 0.4f + (float)(rand() % 10) / 20.0f;
        p->maxLife = p->life;
        p->size = 3.0f + (float)(rand() % 5);
        p->isActive = true;
    }
}

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
        
        p->velocity.y += 200.0f * deltaTime; // Gravité légère
        p->position.x += p->velocity.x * deltaTime;
        p->position.y += p->velocity.y * deltaTime;
        p->velocity.x *= 0.98f;
        p->velocity.y *= 0.98f;
    }
    
    // Compacter le tableau
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

void suika_loadAssets(SuikaGame_St* game)
{
    const char* assetPath = suika_resolveAssetPath(game);
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

    // Initialisation des nouvelles fonctionnalités
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
            game->fruits[i].position.x = game->nextFruitX + (float)(rand() % 7 - 3);  // Incertitude de 3 pixels

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


    Vector2 mousePos = GetMousePosition();
    float minX = SUIKA_CONTAINER_X + game->nextFruit.radius;
    float maxX = SUIKA_CONTAINER_X + SUIKA_CONTAINER_WIDTH - game->nextFruit.radius;
    game->nextFruitX = Clamp(mousePos.x, minX, maxX);
    game->nextFruit.position.x = game->nextFruitX;

    // Mode auto-drop : spawn automatique des fruits (toutes les 0.8s via le cooldown)
    if (game->autoDropEnabled && game->canDrop)
    {
        suika_dropFruit(game);
    }
    // Mode normal : clic pour drop
    else if (!game->autoDropEnabled && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
    {
        suika_dropFruit(game);
        PlaySound(sound_drop);
    }

    suika_updatePhysics(game, deltaTime);
    suika_checkMerging(game);
    suika_checkGameOver(game);
    
    // Mise à jour des particules
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

void suika_updatePhysics(SuikaGame_St* game, float deltaTime)
{
    // Suppress unused parameter warning
    (void)deltaTime;
    
    // ============================================================
    // MOTEUR PHYSIQUE STABLE POUR SUIKA
    // Utilise delta time fixe, plusieurs passes, et correction stable
    // ============================================================
    
    // Constants fixes pour la stabilité
    const float FIXED_DT = 0.016f;          // ~60 FPS fixes
    const int COLLISION_PASSES = 8;          // 8 itérations pour stabilité
<<<<<<< HEAD
    const float GRAVITY = 900.0f;            // Gravité
=======
    /* FIX: use game->gravity (set to 800.0f in suika_init) instead of a local
     * constant of 900.0f that was silently overriding it and causing the physics
     * to run with a different gravity than configured. The struct field was written
     * but never read, making the tunable gravity value ineffective. */
    const float GRAVITY = game->gravity;
>>>>>>> 3777fd6 (- add : new 3D golf game)
    const float DAMPING = 0.999f;            // Évite les tremblements
    const float MAX_VELOCITY = 2000.0f;      // Limite de vitesse
    const float WALL_BOUNCE = 0.3f;           // Rebond sur les murs
    const float FRICTION = 0.95f;            // Friction au sol (réduite pour plus de glissement)
<<<<<<< HEAD
    
=======

    /* FIX: apply velocity damping once per frame BEFORE the passes loop.
     * Previously DAMPING (0.999) and angular damping (0.99) were multiplied
     * inside the loop, making them execute COLLISION_PASSES (8) times per frame.
     * Effective per-frame damping was 0.999^8 ≈ 0.992 and 0.99^8 ≈ 0.923 —
     * far stronger than intended and making fruits stop unnaturally fast. */
    for (int i = 0; i < SUIKA_MAX_FRUITS; i++)
    {
        if (!game->fruits[i].isActive || game->fruits[i].isMerging) continue;
        Fruit_St* f = &game->fruits[i];
        f->velocity.x      *= DAMPING;
        f->velocity.y      *= DAMPING;
        f->angularVelocity *= 0.99f;
    }

>>>>>>> 3777fd6 (- add : new 3D golf game)
    // Appliquer plusieurs passes de physique pour la stabilité
    for (int pass = 0; pass < COLLISION_PASSES; pass++)
    {
        // Mise à jour de chaque fruit
        for (int i = 0; i < SUIKA_MAX_FRUITS; i++)
        {
            if (!game->fruits[i].isActive || game->fruits[i].isMerging)
                continue;

            Fruit_St* f = &game->fruits[i];
            
            // Gravité (une fraction par passe)
            f->velocity.y += GRAVITY * FIXED_DT / (float)COLLISION_PASSES;
            
            // Limite de vitesse
            if (f->velocity.y > MAX_VELOCITY) f->velocity.y = MAX_VELOCITY;
            if (f->velocity.y < -MAX_VELOCITY) f->velocity.y = -MAX_VELOCITY;
            if (f->velocity.x > MAX_VELOCITY) f->velocity.x = MAX_VELOCITY;
            if (f->velocity.x < -MAX_VELOCITY) f->velocity.x = -MAX_VELOCITY;
<<<<<<< HEAD
            
            // Damping
            f->velocity.x *= DAMPING;
            f->velocity.y *= DAMPING;
            f->angularVelocity *= 0.99f;
=======
>>>>>>> 3777fd6 (- add : new 3D golf game)

            // Déplacement
            f->position.x += f->velocity.x * FIXED_DT / (float)COLLISION_PASSES;
            f->position.y += f->velocity.y * FIXED_DT / (float)COLLISION_PASSES;
            f->rotation += f->angularVelocity * FIXED_DT / (float)COLLISION_PASSES;
            
            // Collision avec les murs
            float minX = SUIKA_CONTAINER_X + f->radius;
            float maxX = SUIKA_CONTAINER_X + SUIKA_CONTAINER_WIDTH - f->radius;
            float maxY = SUIKA_CONTAINER_Y + SUIKA_CONTAINER_HEIGHT - f->radius;

            // Mur gauche
            if (f->position.x < minX)
            {
                f->position.x = minX;
                f->velocity.x = -f->velocity.x * WALL_BOUNCE;
            }
            // Mur droit
            else if (f->position.x > maxX)
            {
                f->position.x = maxX;
                f->velocity.x = -f->velocity.x * WALL_BOUNCE;
            }
            // Sol
            if (f->position.y > maxY)
            {
                f->position.y = maxY;
                f->velocity.y = -f->velocity.y * WALL_BOUNCE;
<<<<<<< HEAD
                f->velocity.x *= FRICTION;
                
                // Roulement sur le sol : v = r * omega
                f->angularVelocity = f->velocity.x / f->radius;
=======
                /* FIX: floor friction was applied every collision pass (up to 8x per frame),
                 * giving an effective per-frame FRICTION of 0.95^8 ≈ 0.66 instead of 0.95.
                 * Guard with pass == 0 so it is applied only once per physics frame. */
                if (pass == 0)
                {
                    f->velocity.x *= FRICTION;
                    // Roulement sur le sol : v = r * omega
                    f->angularVelocity = f->velocity.x / f->radius;
                }
>>>>>>> 3777fd6 (- add : new 3D golf game)

                // Arrêt si vitesse très faible
                if (fabsf(f->velocity.y) < 10.0f)
                    f->velocity.y = 0;
            }
        }
        
        // Résolution des collisions fruit-fruit (une passe)
        for (int i = 0; i < SUIKA_MAX_FRUITS; i++)
        {
            if (!game->fruits[i].isActive || game->fruits[i].isMerging)
                continue;

            Fruit_St* a = &game->fruits[i];

            for (int j = i + 1; j < SUIKA_MAX_FRUITS; j++)
            {
                if (!game->fruits[j].isActive || game->fruits[j].isMerging)
                    continue;

                Fruit_St* b = &game->fruits[j];

                // Collision cercle-cercle
                float dx = b->position.x - a->position.x;
                float dy = b->position.y - a->position.y;
                float dist2 = dx*dx + dy*dy;
                float minDist = a->radius + b->radius;
                
                if (dist2 < minDist * minDist && dist2 > 0.0001f)
                {
                    float dist = sqrtf(dist2);
                    float overlap = minDist - dist;
                    
                    // Correction de pénétration stable
                    float nx = dx / dist;
                    float ny = dy / dist;
                    
                    // Séparation égale
                    float correction = overlap * 0.5f;
                    a->position.x -= nx * correction;
                    a->position.y -= ny * correction;
                    b->position.x += nx * correction;
                    b->position.y += ny * correction;
                    
                    // Échange de vélocité (impulsion simple)
                    float dvx = b->velocity.x - a->velocity.x;
                    float dvy = b->velocity.y - a->velocity.y;
                    float velAlongNormal = dvx * nx + dvy * ny;
                    
                    if (velAlongNormal < 0)
                    {
                        float impulse = velAlongNormal * 0.5f;
                        a->velocity.x += impulse * nx;
                        a->velocity.y += impulse * ny;
                        b->velocity.x -= impulse * nx;
                        b->velocity.y -= impulse * ny;
                    }
                    
                    // Roulement induit par contact : la composante tangentielle transfère la rotation
                    float tx = -ny;
                    float relTangVel = (b->velocity.x - a->velocity.x) * tx +
                                      (b->velocity.y - a->velocity.y) * (-nx);
                    float spinTransfer = relTangVel * 0.05f;
                    a->angularVelocity -= spinTransfer / a->radius;
                    b->angularVelocity += spinTransfer / b->radius;
                }
            }
        }
    }
    
    // Supprimer les fruits inactifs à la fin
    for (int i = 0; i < SUIKA_MAX_FRUITS; i++)
    {
        if (!game->fruits[i].isActive)
        {
            // Compacter le tableau si nécessaire
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

<<<<<<< HEAD
=======
                        /* FIX: mark both fruits as isMerging BEFORE searching for a free
                         * slot. Without this, if the pool is full the merge silently fails
                         * and the two fruits remain active and overlapping — re-triggering
                         * this collision check every frame until a slot becomes free, which
                         * could duplicate score events. Flagging them here excludes them
                         * from further collision checks this frame regardless of outcome. */
                        f1->isMerging = true;
                        f2->isMerging = true;

                        bool slotFound = false;
>>>>>>> 3777fd6 (- add : new 3D golf game)
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
<<<<<<< HEAD
                                
=======

>>>>>>> 3777fd6 (- add : new 3D golf game)
                                // Effet visuel de particules lors de la fusion
                                suika_spawnMergeParticles(game, midPos, props->color);

                                PlaySound(sound_merge);
<<<<<<< HEAD
                                
                                f1->isActive = false;
                                f2->isActive = false;

                                return;
                            }
                        }
=======

                                f1->isActive = false;
                                f2->isActive = false;

                                slotFound = true;
                                return;
                            }
                        }

                        /* FIX: pool was full — reset isMerging so the fruits are still
                         * considered for physics and can be merged next frame once a slot
                         * frees up (e.g. a watermelon merge clears two slots). */
                        if (!slotFound)
                        {
                            f1->isMerging = false;
                            f2->isMerging = false;
                        }
>>>>>>> 3777fd6 (- add : new 3D golf game)
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
<<<<<<< HEAD
=======
    /* FIX: clear particles — without this, particles spawned during the previous
     * game (merges, etc.) stay alive after reset and render over the new game. */
    game->particleCount = 0;
>>>>>>> 3777fd6 (- add : new 3D golf game)

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

    // Fallback: dessiner un cercle de couleur si la texture n'est pas chargée
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
    // Fond uni sombre avec dégradé subtil (plus performant qu ligne par ligne)
    Color topColor = (Color){20, 20, 40, 255};
    Color bottomColor = (Color){50, 30, 70, 255};
    
    // Dessiner le fond en deux rectangles pour un effet de dégradé
    Rectangle topRect = {0, 0, SUIKA_SCREEN_WIDTH, SUIKA_SCREEN_HEIGHT / 2};
    Rectangle bottomRect = {0, SUIKA_SCREEN_HEIGHT / 2, SUIKA_SCREEN_WIDTH, SUIKA_SCREEN_HEIGHT / 2};
    
    DrawRectangleRec(topRect, topColor);
    DrawRectangleRec(bottomRect, bottomColor);
}

static void suika_drawContainer(void)
{
    Rectangle container = {SUIKA_CONTAINER_X, SUIKA_CONTAINER_Y, 
                           SUIKA_CONTAINER_WIDTH, SUIKA_CONTAINER_HEIGHT};
    
    // Fond du conteneur avec dégradé léger
    Color bgColor = (Color){35, 35, 55, 230};
    DrawRectangleRec(container, bgColor);
    
    // Bordure intérieure sombre
    Color innerBorder = (Color){50, 50, 70, 255};
    DrawRectangleLinesEx(container, 2.0f, innerBorder);
    
    // Bordure extérieure plus brillante
    Color outerBorder = (Color){120, 120, 160, 255};
    Rectangle outerRect = {container.x - 2, container.y - 2, container.width + 4, container.height + 4};
    DrawRectangleLinesEx(outerRect, 4.0f, outerBorder);
    
    // Ligne de dépôt avec effet de glow
    Color dropLineColor = (Color){255, 120, 120, 180};
    DrawLine(SUIKA_CONTAINER_X + 5, SUIKA_DROP_LINE_Y, 
             SUIKA_CONTAINER_X + SUIKA_CONTAINER_WIDTH - 5, SUIKA_DROP_LINE_Y, 
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
    
    // Dessiner les particules
    suika_drawParticles(game);
    
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


