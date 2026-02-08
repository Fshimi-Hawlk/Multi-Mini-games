/**
 * @file main.c
 * @author LeandreB8 - Fshimi Hawlk
 * @date 2026-02-08
 * @brief Program entry point – lobby main loop and game scene manager.
 *
 * This file contains the top-level application loop.
 * It initializes the window and shared resources, runs the lobby,
 * and switches to individual games when triggered (e.g. collision with zone).
 *
 * Games are loaded on demand via their API (e.g. gameNameAPI.h) and run
 * in the same process/window. No separate executables are spawned.
 */

#include "core/game.h"              // GameScene_Et, general game types
#include "ui/app.h"                 // UI helpers (skin menu, buttons, etc.)
#include "ui/game.h"                // Player drawing, platform logic

#include "utils/globals.h"          // Global constants (WINDOW_WIDTH, etc.)

#include "APIs/gameNameAPI.h"       // Example game API – replace/add others

// ─────────────────────────────────────────────────────────────────────────────
// Global / shared state
// ─────────────────────────────────────────────────────────────────────────────

/** Player controlled by the user in the lobby */
static Player_st player = {
    .position   = {0, 250},
    .radius     = 20,
    .texture    = NULL,
    .angle      = 0,
    .velocity   = {0, 0},
    .onGround   = false,
    .nbJumps    = 0,
    .coyoteTime = 0.1f,
    .coyoteTimer= 0.1f,
    .jumpBuffer = 0.2f
};

/** Camera following the player in 2D mode */
static Camera2D cam = {
    .offset = {WINDOW_WIDTH / 2.0f, WINDOW_HEIGHT / 2.0f},
    .zoom   = 1.0f,
};

/** Hitbox that triggers the GameName mini-game when player collides */
static Rectangle tetrisHitbox = {
    .x      = 600,
    .y      = -150,
    .width  = 75,
    .height = 75
};

/** Current active scene (lobby or one of the mini-games) */
static GameScene_Et currentScene = GAME_SCENE_LOBBY;

/** Flag: game needs initialization on next frame */
static bool needGameInit = false;

/** Prevents multiple rapid triggers when standing on hitbox */
static bool gameHitGracePeriodActive = false;

/** Example instance of one game – add more for other games */
static GameNameGame_St* gameNameGame = NULL;

// ─────────────────────────────────────────────────────────────────────────────
// Helper functions
// ─────────────────────────────────────────────────────────────────────────────

/**
 * @brief Checks if a game instance is still running.
 * @param game  Pointer to a Game_St-compatible structure
 * @return true if game->running is true, false otherwise
 */
bool isGameRunning(const Game_St* game) {
    return game != NULL && game->running;
}

/**
 * @brief Updates and renders the lobby scene (one frame).
 * @param dt  Frame time delta (from GetFrameTime())
 *
 * Handles player movement, camera, UI menus, and game zone collision detection.
 */
static void lobby_gameLoop(float dt) {
    static float lobbyTextXPos;

    updatePlayer(&player, platforms, platformCount, dt);
    cam.target = player.position;

    toggleSkinMenu();

    if (isTextureMenuOpen) {
        choosePlayerTexture(&player);
    }

    // Collision check with game zone (tetris example)
    if (CheckCollisionCircleRec(player.position, player.radius, tetrisHitbox)) {
        if (!gameHitGracePeriodActive) {
            currentScene = GAME_SCENE_GAME_NAME;
            needGameInit = true;
            gameHitGracePeriodActive = true;
        }
    } else if (gameHitGracePeriodActive) {
        gameHitGracePeriodActive = false;
    }

    BeginDrawing(); {
        ClearBackground(RAYWHITE);

        BeginMode2D(cam); {
            DrawCircle(0, 0, 10, RED);          // Debug origin marker
            drawPlayer(&player);
            drawPlatforms(platforms, platformCount);
            DrawRectangleRec(tetrisHitbox, RED); // Debug hitbox
        } EndMode2D();

        lobbyTextXPos = (WINDOW_WIDTH - MeasureText("Multi-Mini-Games", 20)) / 2.0f;
        DrawText("Multi-Mini-Games", lobbyTextXPos, 20, 20, PURPLE);

        drawSkinButton();

        if (isTextureMenuOpen) {
            drawMenuTextures();
        }
    } EndDrawing();
}

// ─────────────────────────────────────────────────────────────────────────────
// Program entry point
// ─────────────────────────────────────────────────────────────────────────────

int main(void) {
    SetTraceLogLevel(LOG_WARNING);

    // ── Initialization ───────────────────────────────────────────────────────
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE);

    // Load shared UI textures
    logoSkinButton = LoadTexture(IMAGES_PATH "logoSkin.png");

    playerTextures[0] = LoadTexture(IMAGES_PATH "earth.png"); playerTextureCount++;
    playerTextures[1] = LoadTexture(IMAGES_PATH "trollFace.png"); playerTextureCount++;

    cam.target = player.position;

    // ── Main loop ────────────────────────────────────────────────────────────
    Error_Et error = OK;

    while (!WindowShouldClose()) {
        float dt = GetFrameTime();

        switch (currentScene) {
            case GAME_SCENE_LOBBY: {
                lobby_gameLoop(dt);
            } break;

            case GAME_SCENE_GAME_NAME: {
                if (needGameInit) {
                    error = gameName_initGame(&gameNameGame, .fps = 120);
                    needGameInit = false;

                    if (error != OK) {
                        log_fatal("GameName initialization failed: error %d", error);
                        gameName_freeGame(&gameNameGame);
                        currentScene = GAME_SCENE_LOBBY;
                        break;
                    }
                }

                gameName_gameLoop(gameNameGame);

                if (!isGameRunning((Game_St*)gameNameGame)) {
                    gameName_freeGame(&gameNameGame);
                    currentScene = GAME_SCENE_LOBBY;
                }
            } break;

            default:
                log_error("Invalid GameScene_Et value: %d", currentScene);
                break;
        }
    }

    // ── Cleanup ──────────────────────────────────────────────────────────────
    if (gameNameGame != NULL) {
        gameName_freeGame(&gameNameGame);
    }

    for (int i = 0; i < playerTextureCount; i++) {
        UnloadTexture(playerTextures[i]);
    }

    UnloadTexture(logoSkinButton);

    CloseWindow();

    return 0;
}

#define LOGGER_IMPLEMENTATION
#include "logger.h"