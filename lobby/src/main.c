#include "core/game.h"

#include "ui/app.h"
#include "ui/game.h"

#include "utils/globals.h"

#include "APIs/solitaireAPI.h"

typedef enum {
    GAME_SCENE_LOBBY,
    GAME_SCENE_SOLITAIRE,
    GAME_SCENE_COUNT
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

Rectangle solitaireHitbox = {
    .x = 600,
    .y = -150,
    .width = 75,
    .height = 75
};

typedef struct {
    const char* name;
    Rectangle hitbox;
    Color color;
    GameScene_Et scene;
    void* gameInstance;
    bool available;
    void* (*initFunc)(const void*);
    void (*loopFunc)(void*);
    void (*freeFunc)(void*);
    bool (*isRunningFunc)(const void*);
} MiniGame_St;

static MiniGame_St miniGames[] = {
    {
        .name = "SOLITAIRE",
        .hitbox = {600, -150, 75, 75},
        .color = BLUE,
        .scene = GAME_SCENE_SOLITAIRE,
        .gameInstance = NULL,
        .available = false,
        .initFunc = (void*(*)(const void*))solitaire_initGame,
        .loopFunc = (void(*)(void*))solitaire_gameLoop,
        .freeFunc = (void(*)(void*))solitaire_freeGame,
        .isRunningFunc = (bool(*)(const void*))solitaire_isRunning
    },
};

#define MINI_GAME_COUNT (sizeof(miniGames) / sizeof(miniGames[0]))

GameScene_Et currentScene = GAME_SCENE_LOBBY;
bool needGameInit = false;
int nextGameIndex = -1;
bool gameHitGracePeriodActive = false;

void lobby_gameLoop(float dt) {
    srand(time(NULL));

    updatePlayer(&player, platforms, platformCount, dt);
    cam.target = player.position;

    toggleSkinMenu();

    if (isTextureMenuOpen) {
        choosePlayerTexture(&player);
    }

    for (size_t i = 0; i < MINI_GAME_COUNT; i++) {
        if (!miniGames[i].available) continue;
        
        if (CheckCollisionCircleRec(player.position, player.radius, miniGames[i].hitbox)) {
            if (!gameHitGracePeriodActive) {
                nextGameIndex = (int)i;
                currentScene = miniGames[i].scene;
                needGameInit = true;
                gameHitGracePeriodActive = true;
            }
            break;
        }
    }
    
    if (!CheckCollisionCircleRec(player.position, player.radius, 
        (Rectangle){0, 0, 0, 0})) {
        bool stillColliding = false;
        for (size_t i = 0; i < MINI_GAME_COUNT; i++) {
            if (miniGames[i].available && 
                CheckCollisionCircleRec(player.position, player.radius, miniGames[i].hitbox)) {
                stillColliding = true;
                break;
            }
        }
        if (!stillColliding && gameHitGracePeriodActive) {
            gameHitGracePeriodActive = false;
        }
    }

    BeginDrawing(); {
        ClearBackground(RAYWHITE);
        BeginMode2D(cam); {
            DrawCircle(0, 0, 10, RED);
            drawPlayer(&player);
            drawPlatforms(platforms, platformCount);

            for (size_t i = 0; i < MINI_GAME_COUNT; i++) {
                if (miniGames[i].available) {
                    DrawRectangleRec(miniGames[i].hitbox, miniGames[i].color);
                    int textWidth = MeasureText(miniGames[i].name, 12);
                    DrawText(miniGames[i].name, 
                        miniGames[i].hitbox.x + (miniGames[i].hitbox.width - textWidth) / 2, 
                        miniGames[i].hitbox.y + 25, 12, WHITE);
                }
            }
        } EndMode2D();

        DrawText("Multi-Mini-Games", WINDOW_WIDTH / 2 - MeasureText("Multi-Mini-Games", 20) / 2, 20, 20, PURPLE);
        drawSkinButton();
        
        if (isTextureMenuOpen) {
            drawMenuTextures();
        }
    } EndDrawing();
}

static void initMiniGames(void) {
    for (size_t i = 0; i < MINI_GAME_COUNT; i++) {
        if (miniGames[i].initFunc) {
            void* testInstance = miniGames[i].initFunc(NULL);
            if (testInstance) {
                miniGames[i].available = true;
                miniGames[i].freeFunc(testInstance);
            } else {
                miniGames[i].available = false;
            }
        }
    }
}

static void cleanupMiniGames(void) {
    for (size_t i = 0; i < MINI_GAME_COUNT; i++) {
        if (miniGames[i].gameInstance && miniGames[i].freeFunc) {
            miniGames[i].freeFunc(miniGames[i].gameInstance);
            miniGames[i].gameInstance = NULL;
        }
    }
}

int main(void) {
    SetTraceLogLevel(LOG_WARNING);

    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE);

    // Initialize audio device
    InitAudioDevice();

    logoSkinButton = LoadTexture(IMAGES_PATH "logoSkin.png");

    playerTextures[0] = LoadTexture(IMAGES_PATH "earth.png"); playerTextureCount++;
    playerTextures[1] = LoadTexture(IMAGES_PATH "trollFace.png"); playerTextureCount++;
    
    cam.target = player.position;

    initMiniGames();

    while (!WindowShouldClose()) {
        float dt = GetFrameTime();

        switch (currentScene) {
            case GAME_SCENE_LOBBY: {
                lobby_gameLoop(dt);
            } break;

            case GAME_SCENE_SOLITAIRE: {
                if (needGameInit && nextGameIndex >= 0) {
                    MiniGame_St* mg = &miniGames[nextGameIndex];
                    if (mg->initFunc) {
                        mg->gameInstance = mg->initFunc(NULL);
                    }
                    if (!mg->gameInstance) {
                        currentScene = GAME_SCENE_LOBBY;
                    }
                    needGameInit = false;
                }

                if (nextGameIndex >= 0) {
                    MiniGame_St* mg = &miniGames[nextGameIndex];
                    if (mg->gameInstance && mg->loopFunc) {
                        mg->loopFunc(mg->gameInstance);

                        if (mg->isRunningFunc && !mg->isRunningFunc(mg->gameInstance)) {
                            if (mg->freeFunc) mg->freeFunc(mg->gameInstance);
                            mg->gameInstance = NULL;
                            currentScene = GAME_SCENE_LOBBY;
                            nextGameIndex = -1;
                        }
                    }
                }
            } break;

            default: {
                log_error("GameScene_Et");
                currentScene = GAME_SCENE_LOBBY;
            }
        }
    }

    cleanupMiniGames();

    for (int i = 0; i < playerTextureCount; i++)
        UnloadTexture(playerTextures[i]);
    
    UnloadTexture(logoSkinButton);

    CloseWindow();
    return 0;
}

#define LOGGER_IMPLEMENTATION
#include "logger.h"
