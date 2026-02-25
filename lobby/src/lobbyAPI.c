/**
    @file lobbyAPI.c
    @author Fshimi-Hawlk
    @date 2026-02-08
    @date 2026-02-23
    @brief Lobby / hub scene - entry point and central navigation area

    Contributors:
        - LeandreB8:
            - Provided the initial logic for init and the game loop.
        - Fshimi-Hawlk:
            - Moved reworked lobby's initialization, game loop and freeing logic in dedicated `lobbyAPI` files
            - Added documentation

    This module implements the main lobby scene that serves as the central hub
    of the multi mini-game application. The player can:
        - move around a small 2D platformer-style world,
        - open a skin / texture selection menu,
        - collide with designated trigger zones to launch individual mini-games.

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
    Future directions may include UI buttons, portals with animations, etc.

    @note    Most mini-game specific logic is delegated to the corresponding
             scene modules through the function pointer table in SubGameManager.

    @see `core/game.h`       for `choosePlayerTexture()`, `toggleSkinMenu()`, `updatePlayer()`
    @see `ui/app.h`          for `drawMenuTextures()`, `drawSkinButton()`
    @see `ui/game.h`         for `drawPlatforms()`, `drawPlayer()`
    @see `utils/globals.h`   for `logoSkinButton`, `platformCount`, `platforms`
    @see `APIs/generalAPI.h` for `Error_Et`
*/

#include "core/game.h"

#include "ui/app.h"                 // UI helpers (skin menu, buttons, etc.)
#include "ui/game.h"                // Player drawing, platform logic
#include "ui/grass.h"
#include "ui/background.h"
#include "ui/ambiance.h"

#include "setups/app.h"
#include "setups/game.h"
#include "setups/texture.h"

#include "utils/globals.h"

#include "lobbyAPI.h"

static void updateCameraOnWindowResize(LobbyGame_St* const game) {
    const f32 originalWidth  = 800;
    const f32 originalHeight = 600;

    // Always keep camera perfectly centered on the new window size
    game->cam.offset = (Vector2){
        systemSettings.video.width  / 2.0f,
        systemSettings.video.height / 2.0f
    };

    // ── Zoom adaptation when width OR height changes ─────────────────────
    f32 zoomX = (f32)systemSettings.video.width  / originalWidth;
    f32 zoomY = (f32)systemSettings.video.height / originalHeight;

    game->cam.zoom = fminf(zoomX, zoomY);
}

Error_Et lobby_initGame__full(LobbyGame_St** game, LobbyConfigs_St configs) {
    Error_Et error = OK;

    (void) configs; // Configs aren't used yet

    systemSettings = DEFAULT_SYSTEM_SETTING;
    systemSettings.video.title = "Lobby";

    lobby_initApp();

    (*game) = malloc(sizeof(LobbyGame_St));
    if (*game == NULL) return ERROR_ALLOC;

    LobbyGame_St* gameRef = *game;
    memset(gameRef, 0, sizeof(*gameRef));

    /** Player controlled by the user in the lobby */
    gameRef->player = (Player_St) {
        .position   = {0, 250},
        .radius     = 20,
        .coyoteTime = 0.1f,
        .coyoteTimer= 0.1f,
        .jumpBuffer = 0.2f,
        .unlockedTextures = {
            [PLAYER_TEXTURE_DEFAULT] = 1,
            [PLAYER_TEXTURE_EARTH] = 1
        }
    };

    /** Camera following the player in 2D mode */
    gameRef->cam = (Camera2D) {
        .offset = {systemSettings.video.width / 2.0f, systemSettings.video.height / 2.0f},
        .zoom   = 1.0f,
    };

    /** Hitbox that triggers the GameName mini-game when player collides */
    gameRef->subGameManager.gameHitboxes[GAME_SCENE_TETRIS] = (Rectangle) {
        .x      = 425,
        .y      = -450,
        .width  = 75,
        .height = 75,
    };

    /** Current active scene (lobby or one of the mini-games) */
    gameRef->subGameManager.currentScene = GAME_SCENE_LOBBY;
    
    /** Flag: game needs initialization on next frame */
    gameRef->subGameManager.needGameInit = false;
    
    gameRef->playerVisuals.defaultTextureRect = (Rectangle) {
        .x = 20, .y = 60, .width = 50, .height = 50
    };

    skinButtonRect = (Rectangle) {
        .x = systemSettings.video.width - 70,
        .y = systemSettings.video.height / 2.0f - 25,
        .width = 50, 
        .height = 50
    };

    lobby_initGrass();
    lobby_initBackgroundScale();
    updateCameraOnWindowResize(gameRef);

    error = lobby_initTextures(gameRef->playerVisuals.textures);

    return error;
}

Error_Et lobby_gameLoop(LobbyGame_St* const game) {
    if (game == NULL) return ERROR_NULL_POINTER;

    f32 dt = GetFrameTime();
    static f32 lobbyTextXPos;

    gameTime += dt;

    if (IsWindowResized()) {
        systemSettings.video.width = GetScreenWidth();
        systemSettings.video.height = GetScreenHeight();

        skinButtonRect = (Rectangle) {
            .x = systemSettings.video.width - 70,
            .y = systemSettings.video.height / 2.0f - 25,
            .width = 50, 
            .height = 50
        };

        updateCameraOnWindowResize(game);
    }

    if (gameTime > 1.45f) {
        updatePlayer(&game->player, platforms, platformCount, dt);
    }
    
    game->cam.target = game->player.position;

    toggleSkinMenu(game);

    if (game->playerVisuals.isTextureMenuOpen) {
        choosePlayerTexture(&game->player, game);
    }

    updateGrass(&game->player, GetFrameTime(), gameTime, game->cam);
    updateAtmosphericEffects(dt, &game->player, game->cam);

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
            lobby_drawStarryBackground(game->player.position, game->cam);

            drawTree();
            
            drawPlatforms(platforms, platformCount);

            for (u8 i = 1; i < __gameSceneCount; ++i) {
                DrawRectangleRec(game->subGameManager.gameHitboxes[i], RED); // Debug hitbox
            }

            drawPlayer(game);
            
            drawWorldBoundaries(&game->player);
            
            drawGrass(&game->player, game->cam);
            drawAtmosphericEffects();
        } EndMode2D();

        drawScreenEffects(&game->player);

        lobbyTextXPos = (systemSettings.video.width - MeasureText("Multi-Mini-Games", 20)) / 2.0f;
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

    lobby_freeTextures(gameRef->playerVisuals.textures);

    free(gameRef);
    *game = NULL;

    CloseWindow();

    return OK;
}
