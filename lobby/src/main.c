#include "core/game.h"

#include "ui/app.h"
#include "ui/game.h"

#include "utils/globals.h"

#include "APIs/tetrisAPI.h"

typedef enum {
    GAME_SCENE_LOBBY,
    GAME_SCENE_TETRIS,
} GameScene_Et;

typedef struct {
    bool running;
} Game_St;

Player_st player = {
    .position = {0, 250},
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

Camera2D cam = {
    .offset = (Vector2) {WINDOW_WIDTH / 2.0f, WINDOW_HEIGHT / 2.0f},
    .zoom = 1.0f,
};

Rectangle tetrisHitbox = {
    .x = 600,
    .y = -150,
    .width = 75,
    .height = 75
};

GameScene_Et currentScene = GAME_SCENE_LOBBY;
bool needGameInit = false;
bool gameHitGracePeriodActive = false;

bool isGameRunning(const Game_St* game) {
    return game->running;
}

void lobby_gameLoop(float dt) {
    srand(time(NULL));

    updatePlayer(&player, platforms, platformCount, dt);

    cam.target = player.position;

    toggleSkinMenu();

    if (isTextureMenuOpen) {
        choosePlayerTexture(&player);
    }

    if (CheckCollisionCircleRec(player.position, player.radius, tetrisHitbox)) {
        if (!gameHitGracePeriodActive) {
            currentScene = GAME_SCENE_TETRIS;
            needGameInit = true;
            gameHitGracePeriodActive = true;
        }
    } else if (gameHitGracePeriodActive) {
        gameHitGracePeriodActive = false;
    }

    BeginDrawing(); {
        ClearBackground(RAYWHITE);
        BeginMode2D(cam); {
            DrawCircle(0, 0, 10, RED);

            drawPlayer(&player);
            drawPlatforms(platforms, platformCount);

            DrawRectangleRec(tetrisHitbox, RED);
        } EndMode2D();

        DrawText("Multi-Mini-Games", WINDOW_WIDTH / 2 - MeasureText("Multi-Mini-Games", 20) / 2, 20, 20, PURPLE);
        drawSkinButton();
        
        if (isTextureMenuOpen) {
            drawMenuTextures();
        }
    } EndDrawing();
}

int main(void) {
    SetTraceLogLevel(LOG_WARNING);

    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE);

    logoSkinButton = LoadTexture(IMAGES_PATH "logoSkin.png");

    playerTextures[0] = LoadTexture(IMAGES_PATH "earth.png"); playerTextureCount++;
    playerTextures[1] = LoadTexture(IMAGES_PATH "trollFace.png"); playerTextureCount++;
    
    cam.target = player.position;

    TetrisGame_St* tetrisGame = NULL;

    while (!WindowShouldClose()) {
        float dt = GetFrameTime();

        switch (currentScene) {
            case GAME_SCENE_LOBBY: {
                lobby_gameLoop(dt);
            } break;

            case GAME_SCENE_TETRIS: {
                if (needGameInit) {
                    tetrisGame = tetris_initGame();
                    needGameInit = false;
                }

                tetris_gameLoop(tetrisGame);

                if (!isGameRunning((Game_St *) tetrisGame)) {
                    tetris_freeGame(&tetrisGame);
                    currentScene = GAME_SCENE_LOBBY;
                }
            } break;

            default: {
                log_error("GameScene_Et");
            }
        }
    }

    if (tetrisGame != NULL) {
        tetris_freeGame(&tetrisGame);
    }

    for (int i = 0; i < playerTextureCount; i++)
        UnloadTexture(playerTextures[i]);
    
    UnloadTexture(logoSkinButton);

    CloseWindow();
    return 0;
}

#define LOGGER_IMPLEMENTATION
#include "logger.h"