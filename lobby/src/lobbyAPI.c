#include "core/game.h"              // GameScene_Et, general game types

#include "ui/app.h"                 // UI helpers (skin menu, buttons, etc.)
#include "ui/game.h"                // Player drawing, platform logic

#include "utils/globals.h"

#include "lobbyAPI.h"
#include "APIs/generalAPI.h"

Error_Et lobby_initGame__full(LobbyGame_St** game, LobbyConfigs_St configs) {
    Error_Et error;

    SetTraceLogLevel(LOG_WARNING);

    // ── Initialization ───────────────────────────────────────────────────────
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE);

    (void) configs; // Configs aren't used yet

    (*game) = malloc(sizeof(LobbyGame_St));
    if (*game == NULL) return ERROR_ALLOC;

    LobbyGame_St* gameRef = *game;
    memset(gameRef, 0, sizeof(*gameRef));

    /** Player controlled by the user in the lobby */
    gameRef->player = (Player_st) {
        .position   = {0, 250},
        .radius     = 20,
        .coyoteTime = 0.1f,
        .coyoteTimer= 0.1f,
        .jumpBuffer = 0.2f
    };

    /** Camera following the player in 2D mode */
    gameRef->cam = (Camera2D) {
        .offset = {WINDOW_WIDTH / 2.0f, WINDOW_HEIGHT / 2.0f},
        .zoom   = 1.0f,
    };

    /** Hitbox that triggers the GameName mini-game when player collides */
    gameRef->subGameManager.gameHitboxes[GAME_SCENE_TETRIS] = (Rectangle) {
        .x      = 600,
        .y      = -150,
        .width  = 75,
        .height = 75
    };

    /** Current active scene (lobby or one of the mini-games) */
    gameRef->subGameManager.currentScene = GAME_SCENE_LOBBY;

    /** Flag: game needs initialization on next frame */
    gameRef->subGameManager.needGameInit = false;

    // Load shared UI textures
    logoSkinButton = LoadTexture(IMAGES_PATH "logoSkin.png");
    if (!IsTextureValid(logoSkinButton)) {
        log_warn("%s couldn't be loaded proprely.", IMAGES_PATH "logoSkin.png");
        error =  ERROR_TEXTURE_LOAD;
    }


    gameRef->player.visuals.textures[PLAYER_TEXTURE_EARTH] = LoadTexture(IMAGES_PATH "earth.png");
    if (!IsTextureValid(gameRef->player.visuals.textures[PLAYER_TEXTURE_EARTH])) {
        log_warn("%s couldn't be loaded proprely.", IMAGES_PATH "earth.png");
        error =  ERROR_TEXTURE_LOAD;
    }

    gameRef->player.visuals.textures[PLAYER_TEXTURE_TROLL_FACE] = LoadTexture(IMAGES_PATH "trollFace.png");
    if (!IsTextureValid(gameRef->player.visuals.textures[PLAYER_TEXTURE_TROLL_FACE])) {
        log_warn("%s couldn't be loaded proprely.", IMAGES_PATH "trollFace.png");
        error =  ERROR_TEXTURE_LOAD;
    }

    return error;
}

Error_Et lobby_gameLoop(LobbyGame_St* const game) {
    if (game == NULL) return ERROR_NULL_POINTER;

    f32 dt = GetFrameTime();
    static f32 lobbyTextXPos;

    updatePlayer(&game->player, platforms, platformCount, dt);
    game->cam.target = game->player.position;

    toggleSkinMenu();

    if (isTextureMenuOpen) {
        choosePlayerTexture(&game->player);
    }

    // Collision check with game zone
    for (u8 i = 1; i < __gameSceneCount; ++i) {
        if (CheckCollisionCircleRec(game->player.position, game->player.radius, game->subGameManager.gameHitboxes[i])) {
            if (!game->subGameManager.gameHitGracePeriodActive) {
                game->subGameManager.currentScene = i;
                game->subGameManager.needGameInit = true;
                game->subGameManager.gameHitGracePeriodActive = true;
            }
        } else if (game->subGameManager.gameHitGracePeriodActive) {
            game->subGameManager.gameHitGracePeriodActive = false;
        }
    }

    BeginDrawing(); {
        ClearBackground(RAYWHITE);

        BeginMode2D(game->cam); {
            DrawCircle(0, 0, 10, RED);          // Debug origin marker
            drawPlayer(game);
            drawPlatforms(platforms, platformCount);

            for (u8 i = 1; i < __gameSceneCount; ++i) {
                DrawRectangleRec(game->subGameManager.gameHitboxes[i], RED); // Debug hitbox
            }
        } EndMode2D();

        lobbyTextXPos = (WINDOW_WIDTH - MeasureText("Multi-Mini-Games", 20)) / 2.0f;
        DrawText("Multi-Mini-Games", lobbyTextXPos, 20, 20, PURPLE);

        drawSkinButton();

        if (isTextureMenuOpen) {
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
        UnloadTexture(gameRef->player.visuals.textures[i]);
    }

    UnloadTexture(logoSkinButton);

    free(gameRef);
    *game = NULL;

    CloseWindow();

    return OK;
}
