/**
 * @file lobbyAPI.c
 * @author Fshimi-Hawlk
 * @date 2026-02-08
 * @date 2026-02-18
 * @brief Lobby / hub scene - entry point and central navigation area
 *
 * Contributors:
 * - LeandreB8:
 *    - Provided the initial logic for init and the game loop.
 * - Fshimi-Hawlk:
 *    - Moved reworked lobby's initialization, game loop and freeing logic in dedicated `lobbyAPI` files
 *    - Added documentation
 *
 * This module implements the main lobby scene that serves as the central hub
 * of the multi mini-game application. The player can:
 *   - move around a small 2D platformer-style world,
 *   - open a skin / texture selection menu,
 *   - collide with designated trigger zones to launch individual mini-games.
 *
 * Responsibilities:
 *   - Setting seed for rand
 *   - Window & camera initialization
 *   - Player movement & basic platformer physics (via shared updatePlayer())
 *   - Skin / player texture selection UI
 *   - Collision-based mini-game activation with grace period / debounce
 *   - Debug visualization (hitboxes, origin marker)
 *   - Resource loading & cleanup for lobby-specific assets
 *
 * Important data flow:
 *   LobbyGame_St -> owns player, camera, sub-game manager state
 *   SubGameManager -> controls which scene is active and whether initialization
 *                    is required on next frame
 *
 * Mini-game activation is currently performed via rectangular hitbox checks.
 * Future directions may include UI buttons, portals with animations, etc.
 *
 * @note    Most mini-game specific logic is delegated to the corresponding
 *          scene modules through the function pointer table in SubGameManager.
 *
 * @see core/game.h     for `choosePlayerTexture()`, `toggleSkinMenu()`, `updatePlayer()`
 * @see ui/app.h        for `drawMenuTextures()`, `drawSkinButton()`
 * @see ui/game.h       for `drawPlatforms()`, `drawPlayer()`
 * @see utils/globals   for `logoSkinButton`, `platformCount`, `platforms`
 * @see APIs/generalAPI for `Error_Et`
 */

#include "core/game.h"

#include "ui/app.h"
#include "ui/game.h"

#include "utils/globals.h"

#include "lobbyAPI.h"
#include "APIs/generalAPI.h"

Error_Et lobby_initGame__full(LobbyGame_St** game, LobbyConfigs_St configs) {
    Error_Et error;

    srand(time(NULL));
    SetTraceLogLevel(LOG_WARNING);

    // ── Initialization ───────────────────────────────────────────────────────
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE);

    (void) configs; // Configs aren't used yet

    (*game) = malloc(sizeof(LobbyGame_St));
    if (*game == NULL) return ERROR_ALLOC;

    LobbyGame_St* gameRef = *game;
    memset(gameRef, 0, sizeof(*gameRef));

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
    
    /** Player controlled by the user in the lobby */
    gameRef->player = (Player_st) {
        .position   = {0, 250},
        .radius     = 20,
        .coyoteTime = 0.1f,
        .coyoteTimer= 0.1f,
        .jumpBuffer = 0.2f
    };

    gameRef->player.unlockedTextures[PLAYER_TEXTURE_DEFAULT] = 1;
    gameRef->player.unlockedTextures[PLAYER_TEXTURE_EARTH] = 1;

    /** Camera following the player in 2D mode */
    gameRef->cam = (Camera2D) {
        .offset = {WINDOW_WIDTH / 2.0f, WINDOW_HEIGHT / 2.0f},
        .zoom   = 1.0f,
    };
    
    gameRef->playerVisuals.defaultTextureRect = (Rectangle) {
        .x = 20, .y = 60, .width = 50, .height = 50
    };

    // Load shared UI textures
    gameRef->playerVisuals.textures[PLAYER_TEXTURE_EARTH] = LoadTexture(IMAGES_PATH "earth.png");
    if (!IsTextureValid(gameRef->playerVisuals.textures[PLAYER_TEXTURE_EARTH])) {
        log_warn("%s couldn't be loaded proprely.", IMAGES_PATH "earth.png");
        error =  ERROR_TEXTURE_LOAD;
    }
    
    gameRef->playerVisuals.textures[PLAYER_TEXTURE_TROLL_FACE] = LoadTexture(IMAGES_PATH "trollFace.png");
    if (!IsTextureValid(gameRef->playerVisuals.textures[PLAYER_TEXTURE_TROLL_FACE])) {
        log_warn("%s couldn't be loaded proprely.", IMAGES_PATH "trollFace.png");
        error =  ERROR_TEXTURE_LOAD;
    }
    
    logoSkinButton = LoadTexture(IMAGES_PATH "logoSkin.png");
    if (!IsTextureValid(logoSkinButton)) {
        log_warn("%s couldn't be loaded proprely.", IMAGES_PATH "logoSkin.png");
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

    toggleSkinMenu(game);

    if (game->playerVisuals.isTextureMenuOpen) {
        choosePlayerTexture(&game->player, game);
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
            drawPlayer(game, &game->player);
            drawPlatforms(platforms, platformCount);

            for (u8 i = 1; i < __gameSceneCount; ++i) {
                DrawRectangleRec(game->subGameManager.gameHitboxes[i], RED); // Debug hitbox
            }
        } EndMode2D();

        lobbyTextXPos = (WINDOW_WIDTH - MeasureText("Multi-Mini-Games", 20)) / 2.0f;
        DrawText("Multi-Mini-Games", lobbyTextXPos, 20, 20, PURPLE);

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
