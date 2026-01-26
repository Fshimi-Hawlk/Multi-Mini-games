#include "config.h"
#include "constant.h"
#include "global.h"
#include "types.h"

void UpdatePlayer(Player_st* player, Platform_st* platforms, int nbPlatforms, float dt);
void ResolveCircleRectCollision(Player_st* player, Rectangle rect);

void drawPlayer(Player_st* player);
void DrawPlatforms(Platform_st* platforms, int nbPlatforms);

void drawMenuTextures(void);
void choosePlayerTexture(Player_st* player);

void drawSkinButton(void);
void toggleSkinMenu(void);

Camera2D cam = {0};

int main(void) {
    SetTraceLogLevel(LOG_WARNING);

    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Basic Raylib Window");

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
    cam.offset = (Vector2){WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2};
    cam.zoom = 1.0f;

    while (!WindowShouldClose()) {
        float dt = GetFrameTime();

        UpdatePlayer(&player, platforms, platformCount, dt);

        cam.target = player.position;

        toggleSkinMenu();

        if (isTextureMenuOpen) {
            choosePlayerTexture(&player);
        }

        BeginDrawing(); {
            ClearBackground(RAYWHITE);
            BeginMode2D(cam); {
                drawPlayer(&player);
                DrawPlatforms(platforms, platformCount);
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
            player->onGround = false;
            player->coyoteTimer = 0;
            player->nbJumps = 1;
            player->jumpBuffer = 0;
        }
        // Double jump
        else if (player->nbJumps >= 1 && player->nbJumps < MAX_JUMPS) {
            player->velocity.y = -500;
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

void DrawPlatforms(Platform_st* platforms, int nbPlatforms) {
    for (int i = 0; i < nbPlatforms; i++)
        DrawRectangleRounded(platforms[i].rect, platforms[i].roundness, 0, platforms[i].color);
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
