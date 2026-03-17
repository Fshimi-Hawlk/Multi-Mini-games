/**
    @file lobbyAPI.c
    @author Maxime CHAUVEAU
    @date 2026-02-25
    @brief Lobby / hub scene - entry point and central navigation area

    This module implements the main lobby scene that serves as the central hub
    of the multi mini-game application. The player can:
        - move around a small 2D platformer-style world,
        - open a skin / texture selection menu,
        - collide with designated trigger zones to launch individual mini-games (bowling).

    Responsibilities:
        - Setting seed for rand
        - Window & camera initialization
        - Player movement & basic platformer physics (via shared updatePlayer())
        - Skin / player texture selection UI
        - Collision-based mini-game activation with grace period / debounce
        - Debug visualization (hitboxes, origin marker)
        - Resource loading & cleanup for lobby-specific assets

    Important data flow:
      LobbyGame_St -> owns player, camera, sub-game manager state
      SubGameManager -> controls which scene is active and whether initialization
                       is required on next frame

    Mini-game activation is currently performed via rectangular hitbox checks.

    @see `core/game.h`       for `choosePlayerTexture()`, `toggleSkinMenu()`, `updatePlayer()`, `checkGameZones()`
    @see `ui/app.h`          for `drawMenuTextures()`, `drawSkinButton()`, `drawGameHUD()`
    @see `ui/game.h`         for `drawPlatforms()`, `drawPlayer()`, `drawGameZones()`
    @see `utils/globals.h`   for `logoSkinButton`, `platformCount`, `platforms`
    @see `APIs/generalAPI.h` for `Error_Et`
*/

#include "core/game.h"

#include "raylib.h"
#include "ui/app.h"
#include "ui/game.h"

#include "utils/common.h"
#include "utils/globals.h"

#include "lobbyAPI.h"
#include "systemSettings.h"

Error_Et lobby_initGame__full(LobbyGame_St** game, LobbyConfigs_St configs) {
    Error_Et error = OK;

    srand(time(NULL));
    SetTraceLogLevel(LOG_WARNING);

    // ── Initialization ───────────────────────────────────────────────────────
    InitWindow(systemSettings.video.width, systemSettings.video.height, WINDOW_TITLE);
    SetWindowPosition(100, 100);

    (void) configs; // Configs aren't used yet

    systemSettings = DEFAULT_SYSTEM_SETTING;
    systemSettings.video.resizable = true;
    systemSettings.video.title = WINDOW_TITLE;
    error = applySystemSettings();
    if (error != OK) {
        log_error("System settings couldn't be applied correctly");
    }

    (*game) = malloc(sizeof(LobbyGame_St));
    if (*game == NULL) return ERROR_ALLOC;

    LobbyGame_St* gameRef = *game;
    memset(gameRef, 0, sizeof(*gameRef));

    gameRef->base.running = true;
    gameRef->base.paused = false;
    gameRef->base.score = 0;

    /** Hitbox that triggers the Bowling mini-game when player collides */
    gameRef->subGameManager.gameHitboxes[GAME_SCENE_BOWLING] = (Rectangle) {
        .x      = GAME_ZONE_BOWLING_X,
        .y      = GAME_ZONE_BOWLING_Y,
        .width  = GAME_ZONE_BOWLING_WIDTH,
        .height = GAME_ZONE_BOWLING_HEIGHT
    };

    /** Current active scene (lobby or one of the mini-games) */
    gameRef->subGameManager.currentScene = GAME_SCENE_LOBBY;
    
    /** Flag: game needs initialization on next frame */
    gameRef->subGameManager.needGameInit = false;
    
    /** Player controlled by the user in the lobby */
    gameRef->player = (Player_st) {
        .position   = {0, 250},
        .radius     = 20,
        .coyoteTime = COYOTE_TIME,
        .coyoteTimer= COYOTE_TIME,
        .jumpBuffer = JUMP_BUFFER_TIME
    };

    gameRef->player.unlockedTextures[PLAYER_TEXTURE_DEFAULT] = true;
    gameRef->player.unlockedTextures[PLAYER_TEXTURE_EARTH] = true;
    gameRef->player.unlockedTextures[PLAYER_TEXTURE_BOWLING_BALL] = true;

    /** Camera following the player in 2D mode */
    gameRef->cam = (Camera2D) {
        .offset = {systemSettings.video.width / 2.0f, systemSettings.video.height / 2.0f},
        .zoom   = 1.0f,
    };
    
    gameRef->playerVisuals.defaultTextureRect = (Rectangle) {
        .x = 20, .y = 60, .width = 50, .height = 50
    };

    // Load shared UI textures
    gameRef->playerVisuals.textures[PLAYER_TEXTURE_EARTH] = LoadTexture(IMAGES_PATH "earth.png");
    if (!IsTextureValid(gameRef->playerVisuals.textures[PLAYER_TEXTURE_EARTH])) {
        log_warn("%s couldn't be loaded properly.", IMAGES_PATH "earth.png");
        error = ERROR_TEXTURE_LOAD;
    }
    
    gameRef->playerVisuals.textures[PLAYER_TEXTURE_TROLL_FACE] = LoadTexture(IMAGES_PATH "trollFace.png");
    if (!IsTextureValid(gameRef->playerVisuals.textures[PLAYER_TEXTURE_TROLL_FACE])) {
        log_warn("%s couldn't be loaded properly.", IMAGES_PATH "trollFace.png");
        error = ERROR_TEXTURE_LOAD;
    }
    
    gameRef->playerVisuals.textures[PLAYER_TEXTURE_BOWLING_BALL] = LoadTexture(IMAGES_PATH "bowlingBall.png");
    if (!IsTextureValid(gameRef->playerVisuals.textures[PLAYER_TEXTURE_BOWLING_BALL])) {
        log_warn("%s couldn't be loaded properly.", IMAGES_PATH "bowlingBall.png");
    }
    
    logoSkinButton = LoadTexture(IMAGES_PATH "logoSkin.png");
    if (!IsTextureValid(logoSkinButton)) {
        log_warn("%s couldn't be loaded properly.", IMAGES_PATH "logoSkin.png");
        error = ERROR_TEXTURE_LOAD;
    }

    return error;
}

Error_Et lobby_gameLoop(LobbyGame_St* const game) {
    if (game == NULL) return ERROR_NULL_POINTER;

    f32 dt = GetFrameTime();

    updatePlayer(&game->player, platforms, platformCount, dt);
    game->cam.target = game->player.position;

    toggleSkinMenu(game);

    if (game->playerVisuals.isTextureMenuOpen) {
        choosePlayerTexture(&game->player, game);
    }

    // Check collision with game zones
    checkGameZones(game);

    BeginDrawing(); {
        ClearBackground(RAYWHITE);

        BeginMode2D(game->cam); {
            DrawCircle(0, 0, 10, RED);          // Debug origin marker
            drawPlayer(game, &game->player);
            drawPlatforms(platforms, platformCount);
            drawGameZones(gameZones, gameZoneCount);
        } EndMode2D();

        drawGameHUD(game);
        drawSkinButton();

        if (game->playerVisuals.isTextureMenuOpen) {
            drawMenuTextures(game);
        }
    } EndDrawing();

    return OK;
}

Error_Et lobby_freeGame(LobbyGame_St** game) {
    if (game == NULL || *game == NULL) return ERROR_NULL_POINTER;
    LobbyGame_St* gameRef = *game;

    for (u8 i = 1; i < __gameSceneCount; ++i) {
        if (gameRef->subGameManager.miniGames[i] == NULL) continue;
        gameRef->subGameManager.miniGames[i]->freeGame(&gameRef->subGameManager.miniGames[i]);
        gameRef->subGameManager.miniGames[i] = NULL;
    }

    for (u32 i = 1; i < __playerTextureCount; ++i) {
        UnloadTexture(gameRef->playerVisuals.textures[i]);
    }

    UnloadTexture(logoSkinButton);

    free(gameRef);
    *game = NULL;

    CloseWindow();

    return OK;
}
