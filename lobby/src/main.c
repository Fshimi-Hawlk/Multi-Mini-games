#include "config.h"
#include "constant.h"
#include "global.h"
#include "types.h"
#include "audio.h"
#include "textures.h"

void UpdatePlayer(Player_st* player, Platform_st* platforms, int nbPlatforms, float dt);
void ResolveCircleRectCollision(Player_st* player, Rectangle rect);

void drawPlayer(Player_st* player);
void DrawPlatforms(Platform_st* platforms, int nbPlatforms);

void drawMenuTextures(void);
void choosePlayerTexture(Player_st* player);

void drawSkinButton(void);
void toggleSkinMenu(void);

void DrawSceneBackground(float time, Vector2 playerPos);
void DrawTreeAndPlatforms(Platform_st* platforms, int count, Player_st* player, Camera2D camera);
void InitGrass();
void UpdateAndDrawGrass(Player_st* player, float dt, float time, Camera2D camera);

Camera2D cam = {0};

int main(void) {
    srand(time(NULL));

    SetTraceLogLevel(LOG_WARNING);

    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Lobby Multi-Mini-Games");

    lobby_initAudio();
    lobby_initTextures();
    InitGrass();

    logoSkinButton = LoadTexture("assets/logoSkin.png");

    playerTextures[0] = LoadTexture("assets/earth.png"); playerTextureCount++;
    playerTextures[1] = LoadTexture("assets/trollFace.png"); playerTextureCount++;
    
    Player_st player = {
        .position = {0, 300},
        .radius = 20,

        .texture = NULL,
        .angle = 0,

        .velocity = {0, 0},

        .onGround = false,
        .nbJumps = 0,

        .coyoteTime = 0.1f,
        .coyoteTimer = 0.1f,

        .jumpBuffer = 0.2f
    };

    cam.target = player.position;
    cam.offset = (Vector2){WINDOW_WIDTH / 2, WINDOW_HEIGHT * 0.66};
    cam.zoom = 1.0f;

    while (!WindowShouldClose()) {
        float dt = GetFrameTime();
        gameTime += dt;

        if (gameTime > 1)
            UpdatePlayer(&player, platforms, platformCount, dt);

        cam.target = player.position;

        toggleSkinMenu();

        if (isTextureMenuOpen) {
            choosePlayerTexture(&player);
        }

        BeginDrawing(); {
            ClearBackground(RAYWHITE);
            BeginMode2D(cam); {
                DrawSceneBackground(gameTime, player.position);
                DrawTreeAndPlatforms(platforms, platformCount, &player, cam);
                drawPlayer(&player);
                // DrawPlatforms(platforms, platformCount);
            } EndMode2D();
            DrawText("Multi-Mini-Games", WINDOW_WIDTH / 2 - MeasureText("Multi-Mini-Games", 20) / 2, 20, 20, PURPLE);
            drawSkinButton();
            if (isTextureMenuOpen) {
                drawMenuTextures();
            }
        } EndDrawing();
    }

    for (int i = 0; i < playerTextureCount; i++)
        UnloadTexture(playerTextures[i]);
    
    UnloadTexture(logoSkinButton);

    lobby_freeAudio();
    lobby_freeTextures();

    CloseWindow();
    return 0;
}

void UpdatePlayer(Player_st* player, Platform_st* platforms, int nbPlatforms, float dt) {
    // INPUT HORIZONTAL
    if (IsKeyDown(KEY_A))
        player->velocity.x = -300;
    else if (IsKeyDown(KEY_D))
        player->velocity.x = 300;
    else {
        // friction quand aucune touche n'est pressée
        if (player->velocity.x > 0) {
            player->velocity.x -= FRICTION * dt;
            if (player->velocity.x < 0) player->velocity.x = 0;
        } else if (player->velocity.x < 0) {
            player->velocity.x += FRICTION * dt;
            if (player->velocity.x > 0) player->velocity.x = 0;
        }
    }

    // Rotation en fonction de la direction
    if (player->velocity.x > 0) {
        player->angle += 360 * dt; // tourner dans le sens horaire
    }
    else if (player->velocity.x < 0) {
        player->angle -= 360 * dt; // tourner dans le sens anti-horaire
    }

    // INPUT JUMP -> buffer
    if (IsKeyPressed(KEY_SPACE)) {
        player->jumpBuffer = JUMP_BUFFER_TIME;
    }
    else if (player->jumpBuffer > 0) {
        player->jumpBuffer -= dt;
        if (player->jumpBuffer < 0)
            player->jumpBuffer = 0;
    }

    // GRAVITÉ
    player->velocity.y += 1200 * dt;

    // COLLISIONS
    player->position.x += player->velocity.x * dt;
    player->position.y += player->velocity.y * dt;
    player->onGround = false;

    // Empêcher de sortir à gauche
    if (player->position.x - player->radius < -X_LIMIT) {
        player->position.x = -X_LIMIT + player->radius;
        player->velocity.x = 0;
    }
    // Empêcher de sortir à droite
    if (player->position.x + player->radius > X_LIMIT) {
        player->position.x = X_LIMIT - player->radius;
        player->velocity.x = 0;
    }

    for (int i = 0; i < nbPlatforms; i++) {
        ResolveCircleRectCollision(player, platforms[i].rect);
    }

    // COYOTE TIME
    if (player->onGround) {
        player->coyoteTimer = COYOTE_TIME;
        player->nbJumps = 0;
    }
    else {
        player->coyoteTimer -= dt;
        if (player->coyoteTimer < 0)
            player->coyoteTimer = 0;
    }

    // JUMP (buffer + coyote + double jump)
    if (player->jumpBuffer > 0) {
        // Jump sol ou coyote
        if (player->onGround || player->coyoteTimer > 0) {
            player->velocity.y = -500;

            PlaySound(sound_jump);

            player->onGround = false;
            player->coyoteTimer = 0;
            player->nbJumps = 1;
            player->jumpBuffer = 0;
        }
        // Double jump
        else if (player->nbJumps >= 1 && player->nbJumps < MAX_JUMPS) {
            player->velocity.y = -500;

            if (rand() % 1000 == 0) {
                PlaySound(meme);
            }
            else {
                PlaySound(sound_doubleJump);
            }

            player->nbJumps++;
            player->jumpBuffer = 0;
        }
    }
}

void ResolveCircleRectCollision(Player_st* player, Rectangle rect) {
    // point le plus proche du cercle sur le rectangle
    float closestX = Clamp(player->position.x, rect.x, rect.x + rect.width);
    float closestY = Clamp(player->position.y, rect.y, rect.y + rect.height);

    float dx = player->position.x - closestX;
    float dy = player->position.y - closestY;

    float distSq = dx*dx + dy*dy;
    float r = player->radius;

    if (distSq >= r * r)
        return;

    float dist = sqrtf(distSq);
    if (dist == 0)
        return;

    float penetration = r - dist;

    float nx = dx / dist;
    float ny = dy / dist;

    // correction position
    player->position.x += nx * penetration;
    player->position.y += ny * penetration;

    // résolution vitesse selon l’axe dominant
    if (fabsf(nx) > fabsf(ny)) {
        player->velocity.x = 0;
    }
    else {
        player->velocity.y = 0;

        // sol
        if (ny < 0) {
            player->onGround = true;
            player->nbJumps = 0;
            player->coyoteTimer = COYOTE_TIME;
        }
    }
}

void drawPlayer(Player_st* player) {
    if (player->texture == NULL)
        DrawCircleV(player->position, player->radius, BLUE);
    else
        DrawTexturePro(
            *player->texture,
            (Rectangle){0, 0, (float)player->texture->width, (float)player->texture->height},  // source
            (Rectangle){
                player->position.x,
                player->position.y,
                player->radius * 2,
                player->radius * 2
            }, // destination
            (Vector2){player->radius, player->radius}, // origine du pivot
            player->angle, // angle en degrés
            WHITE
        );
}

void drawMenuTextures(void) {
    Rectangle destRect = defaultPlayerTextureRect;

    DrawText("choose your skin :", 20, 40, 20, DARKGRAY);
    for (int i = 0; i < playerTextureCount; i++) {
        destRect.x = 20 + i * 60;
        DrawTexturePro(
            playerTextures[i],
            (Rectangle){0, 0, playerTextures[i].width, playerTextures[i].height}, // source
            destRect, // destination
            (Vector2){0, 0},
            0,
            WHITE
        );
    }
}

void choosePlayerTexture(Player_st* player) {
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        Vector2 mousePos = GetMousePosition();
        Rectangle destRect = defaultPlayerTextureRect;
    
        for (int i = 0; i < playerTextureCount; i++) {
            destRect.x = 20 + i * 60;
            if (CheckCollisionPointRec(mousePos, destRect)) {
                player->texture = &playerTextures[i];
                isTextureMenuOpen = false;
                break;
            }
        }
    }
    if (IsKeyPressed(KEY_ONE)) {
        player->texture = &playerTextures[0];
        isTextureMenuOpen = false;
    }
    if (IsKeyPressed(KEY_TWO)) {
        player->texture = &playerTextures[1];
        isTextureMenuOpen = false;
    }
}

void drawSkinButton(void) {
    DrawTexturePro(
        logoSkinButton,
        (Rectangle){0, 0, logoSkinButton.width, logoSkinButton.height},
        skinButtonRect,
        (Vector2){0, 0},
        0,
        WHITE
    );
}

void toggleSkinMenu(void) {
    if ((IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && CheckCollisionPointRec(GetMousePosition(), skinButtonRect)) || IsKeyPressed(KEY_P)) {
        isTextureMenuOpen = !isTextureMenuOpen;
    }
}

void DrawWorldBoundaries(Player_st* player) {
    float limitL = -X_LIMIT; 
    float limitR = X_LIMIT;  
    
    float wallTop = -SKY_HEIGHT;
    float wallHeight = SKY_HEIGHT + GROUND_Y + 1000; 
    float detectionRange = 400.0f;

    float distL = player->position.x - limitL;
    float distR = limitR - player->position.x;
    
    if (distL < detectionRange && distL >= 0) {
        float alphaFactor = 1.0f - (distL / detectionRange); 
        unsigned char alpha = (unsigned char)(alphaFactor * 160.0f);
        
        DrawRectangleGradientH(limitL, wallTop, 150, wallHeight, 
                               (Color){255, 255, 255, alpha}, 
                               (Color){255, 255, 255, 0});
        DrawLineEx((Vector2){limitL, wallTop}, (Vector2){limitL, wallTop + wallHeight}, 3.0f, (Color){255, 255, 255, alpha});
    }
    
    if (distR < detectionRange && distR >= 0) {
        float alphaFactor = 1.0f - (distR / detectionRange);
        unsigned char alpha = (unsigned char)(alphaFactor * 160.0f);

        DrawRectangleGradientH(limitR - 150, wallTop, 150, wallHeight, 
                               (Color){255, 255, 255, 0}, 
                               (Color){255, 255, 255, alpha});
        DrawLineEx((Vector2){limitR, wallTop}, (Vector2){limitR, wallTop + wallHeight}, 3.0f, (Color){255, 255, 255, alpha});
    }
}

void DrawSceneBackground(float time, Vector2 playerPos) {
    float parallaxFactor = 0.15f; 
    
    float pX = playerPos.x * parallaxFactor;
    float pY = playerPos.y * parallaxFactor;

    Color skyTop = (Color){ 25, 84, 157, 255 };    
    Color skyMid = (Color){ 125, 195, 230, 255 };  
    Color skyLow = (Color){ 210, 240, 255, 255 };  

    float skyLeft = (playerPos.x - SKY_WIDTH / 2.0f) + pX;
    
    DrawRectangleGradientV(
        skyLeft, 
        -SKY_HEIGHT + pY, 
        SKY_WIDTH, 
        SKY_HEIGHT, 
        skyTop, 
        skyMid
    );
    
    DrawRectangleGradientV(
        skyLeft, 
        0 + pY, 
        SKY_WIDTH, 
        GROUND_Y + 1000,
        skyMid, 
        skyLow
    );
}

void DrawTreeAndPlatforms(Platform_st* platforms, int count, Player_st* player, Camera2D camera) {
    if (texTree.id > 0) { 
        float treeScale = 0.7f; 
        float drawWidth = (float)texTree.width * treeScale;
        float drawHeight = (float)texTree.height * treeScale;

        Vector2 treePos = { -drawWidth / 2.0f, GROUND_Y - drawHeight + 350}; 

        DrawTexturePro(texTree, 
            (Rectangle){0, 0, (float)texTree.width, (float)texTree.height},
            (Rectangle){treePos.x, treePos.y, drawWidth, drawHeight}, 
            (Vector2){0,0}, 0, WHITE);
    }

    DrawWorldBoundaries(player);

    UpdateAndDrawGrass(player, GetFrameTime(), gameTime, camera);
    for (int i = 0; i < count; i++) {
        Rectangle r = platforms[i].rect;

        if (platforms[i].idTex == PLATFORM_TEXTURE_WOODPLANK_ID) {
            Texture2D tex = platformTextures[platforms[i].idTex];

            float offsetX = (float)((int)(r.x * 0.5f) % (tex.width - (int)r.width));
            float offsetY = (float)((int)(r.y * 0.3f) % (tex.height - (int)r.height));
            
            if (offsetX < 0) offsetX = 0;
            if (offsetY < 0) offsetY = 0;

            Rectangle sourceRec = { offsetX, offsetY, r.width, r.height };
            
            // Ombre
            DrawRectangle(r.x - 1, r.y - 1, r.width + 2, r.height + 2, BLACK);

            // Plateforme
            DrawTextureRec(tex, sourceRec, (Vector2){r.x, r.y}, WHITE);
        }
    }
}

void InitGrass() {
    Rectangle floor = platforms[0].rect; 
    grassCount = 0;

    float stepX = 3.0f;
    float stepY = 5.0f; 

    for (float y = floor.y; y < floor.y + 200.0f; y += stepY) {
        for (float x = -X_LIMIT - 500; x < X_LIMIT + 500; x += stepX) {
            
            if (grassCount >= MAX_GRASS_BLADES) break;

            float offX = (float)(rand() % 15) - 7.5f;
            float offY = (float)(rand() % 10);

            float baseHeight;
            int type = rand() % 10;
            if (type < 7) baseHeight = 5.0f + (rand() % 10);
            else if (type < 9) baseHeight = 15.0f + (rand() % 15);
            else baseHeight = 2.0f + (rand() % 4);

            float depth = (y - floor.y) / floor.height;
            float colorVar = (float)(rand() % 35);

            grassBlades[grassCount] = (GrassBlade_st){
                .position = { x + offX, y + offY },
                .height = baseHeight,
                .angle = 0.0f,
                .velocity = 0.0f,
                .color = (Color){ 
                    (unsigned char)Clamp(35 + colorVar - (depth * 15), 10, 255),
                    (unsigned char)Clamp(90 + colorVar - (depth * 70), 20, 180),
                    (unsigned char)Clamp(25 - (depth * 10), 5, 255),
                    255 
                }
            };
            grassCount++;
        }
    }
}

void UpdateAndDrawGrass(Player_st* player, float dt, float time, Camera2D camera) {
    float skyLeft = player->position.x - SKY_WIDTH / 2.0f;
    DrawRectangleGradientV(
        skyLeft, 
        GROUND_Y, 
        SKY_WIDTH, 
        1000, 
        (Color){45, 35, 25, 255}, 
        (Color){20, 15, 10, 255}
    );
    float viewL = camera.target.x - (WINDOW_WIDTH / 2) - 100;
    float viewR = camera.target.x + (WINDOW_WIDTH / 2) + 100;
    float viewT = camera.target.y - (WINDOW_HEIGHT / 2) - 100;
    float viewB = camera.target.y + (WINDOW_HEIGHT / 2) + 100;

    for (int i = 0; i < grassCount; i++) {
        GrassBlade_st* b = &grassBlades[i];

        if (b->position.x < viewL || b->position.x > viewR || 
            b->position.y < viewT || b->position.y > viewB) continue;

        // Vent
        float windBase = sinf(time * 5.0f + b->position.x * 0.05f + b->position.y * 0.02f) * 0.15f;

        // Ressort
        float stiffness = 50.0f;
        float damping = 0.85f;
        
        // Force de rappel vers angle du vent
        float force = -stiffness * (b->angle - windBase);
        b->velocity += force * dt;
        b->velocity *= damping;
        b->angle += b->velocity * dt;

        // Interaction joueur
        float dx = player->position.x - b->position.x;
        float dy = player->position.y - b->position.y;
        float distSq = dx*dx + dy*dy;

        if (distSq > 360000.0f) { 
            b->angle = sinf(time * 2.0f + b->position.x * 0.05f) * 0.1f; 
        }

        if (distSq < 2000.0f) { // Le joueur repousse l'herbe
            b->velocity -= (dx * 1.5f);
        }

        // Sécurité anti-rotation
        b->angle = Clamp(b->angle, -1.0f, 1.0f);

        // Rendu
        Vector2 tip = {
            b->position.x + sinf(b->angle) * b->height,
            b->position.y - cosf(b->angle) * b->height
        };

        float thickness = (b->height > 12.0f) ? 3.0f : 2.0f;

        DrawLineEx(b->position, tip, thickness, b->color); 

        if (b->height > 20.0f) {
            DrawCircleV(b->position, 3, (Color){ 0, 0, 0, 60 });
        }
    }
}