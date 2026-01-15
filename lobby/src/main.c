#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "raylib.h"

#include "config.h"
#include "constant.h"

typedef struct {
    Vector2 position;
    Vector2 velocity;
    Rectangle hitbox;
    bool onGround;
} Player_st;

typedef struct {
    Rectangle rect;
} Platform_st;

Platform_st platforms[] = {
    // Sol principal
    {{-500, 500, 3000, 40}},

    // Paliers bas
    {{200, 430, 180, 25}},
    {{450, 380, 160, 25}},
    {{680, 340, 140, 25}},

    // Zone centrale
    {{900, 420, 200, 30}},
    {{1150, 360, 180, 30}},
    {{1400, 300, 160, 30}},

    // Plateformes hautes
    {{600, 250, 120, 25}},
    {{850, 200, 120, 25}},
    {{1100, 160, 120, 25}},

    // Murs (pour tester collisions horizontales)
    {{1600, 380, 30, 160}},
    {{1800, 300, 30, 200}},
};
int platformCount = sizeof(platforms) / sizeof(platforms[0]);

void UpdatePlayer(Player_st* player, Platform_st* platforms, int nbPlatforms, float dt);
void UpdateHitbox(Player_st* player);
void HandleHorizontalCollisions(Player_st* player, Platform_st* platforms, int nbPlatforms);
void HandleVerticalCollisions(Player_st* player, Platform_st* platforms, int nbPlatforms, float prevY);

void drawPlayer(Player_st* player);
void DrawPlatforms(Platform_st* platforms, int nbPlatforms);

int main() {
    SetTraceLogLevel(LOG_WARNING);

    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Basic Raylib Window");
    
    Player_st player = {
        .position = {200, 300}, // position = pieds du joueur
        .velocity = {0, 0},
        .hitbox = {0, 0, 32, 64}, // largeur, hauteur
        .onGround = false
    };
    UpdateHitbox(&player);

    Camera2D cam = {0};
    cam.target = player.position;
    cam.offset = (Vector2){WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2};
    cam.zoom = 1.0f;

    while (!WindowShouldClose()) {
        float dt = GetFrameTime();

        UpdatePlayer(&player, platforms, platformCount, dt);

        cam.target = player.position;

        BeginDrawing();
            ClearBackground(RAYWHITE);
            BeginMode2D(cam);
                drawPlayer(&player);
                DrawPlatforms(platforms, platformCount);
            EndMode2D();
        EndDrawing();
    }
    
    CloseWindow();
    return 0;
}

void UpdatePlayer(Player_st* player, Platform_st* platforms, int nbPlatforms, float dt) {
    // INPUT
    if (IsKeyDown(KEY_A)) player->velocity.x = -300;
    else if (IsKeyDown(KEY_D)) player->velocity.x = 300;
    else player->velocity.x = 0;

    if (IsKeyPressed(KEY_SPACE) && player->onGround)
        player->velocity.y = -500;

    // GRAVITÉ
    player->velocity.y += 1200 * dt;

    // X
    player->position.x += player->velocity.x * dt;
    UpdateHitbox(player);
    HandleHorizontalCollisions(player, platforms, nbPlatforms);

    // Y
    float prevY = player->position.y;
    player->position.y += player->velocity.y * dt;
    UpdateHitbox(player);
    HandleVerticalCollisions(player, platforms, nbPlatforms, prevY);
}

void UpdateHitbox(Player_st* player) {
    player->hitbox.x = player->position.x - player->hitbox.width / 2;
    player->hitbox.y = player->position.y - player->hitbox.height;
}

void HandleHorizontalCollisions(Player_st* player, Platform_st* platforms, int nbPlatforms) {
    for (int i = 0; i < nbPlatforms; i++) {
        if (CheckCollisionRecs(player->hitbox, platforms[i].rect)) {
            if (player->velocity.x > 0) { // va à droite
                player->position.x = platforms[i].rect.x - player->hitbox.width / 2;
            }
            else if (player->velocity.x < 0) { // va à gauche
                player->position.x = platforms[i].rect.x + platforms[i].rect.width + player->hitbox.width / 2;
            }

            player->velocity.x = 0;
            UpdateHitbox(player);
        }
    }
}

void HandleVerticalCollisions(Player_st* player, Platform_st* platforms, int nbPlatforms, float prevY) {
    player->onGround = false;

    for (int i = 0; i < nbPlatforms; i++) {
        if (CheckCollisionRecs(player->hitbox, platforms[i].rect)) {
            float playerTopPrev = prevY - player->hitbox.height;
            float playerBottomPrev = prevY;

            // chute → sol
            if (player->velocity.y > 0 && playerBottomPrev <= platforms[i].rect.y) {
                player->position.y = platforms[i].rect.y;
                player->velocity.y = 0;
                player->onGround = true;
            }

            // saut → plafond
            else if (player->velocity.y < 0 &&
                     playerTopPrev >= platforms[i].rect.y + platforms[i].rect.height) {

                player->position.y = platforms[i].rect.y + platforms[i].rect.height + player->hitbox.height;
                player->velocity.y = 0;
            }

            UpdateHitbox(player);
        }
    }
}

void drawPlayer(Player_st* player) {
    DrawRectangle(player->hitbox.x, player->hitbox.y, player->hitbox.width, player->hitbox.height, BLUE);
}

void DrawPlatforms(Platform_st* platforms, int nbPlatforms) {
    for (int i = 0; i < nbPlatforms; i++)
        DrawRectangleRec(platforms[i].rect, GREEN);
}
