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

#include "APIs/generalAPI.h"
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

#include "sharedUtils/mathUtils.h"

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

    game->cam.zoom = min(zoomX, zoomY);
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

    // Hitboxes posées sur le sol (floor top = y:500, hitbox height = 75 → y:425)
    // Réparties en x de -350 à +350 pour couvrir toute la zone de jeu.

    GameCollisionZone_St gameHitboxes[__miniGameIdCount] = {
        [MINI_GAME_ID_TETRIS] = {
            .name = "Tetris",
            .color = {0, 120, 220, 200},
            .hitbox = {
                .x      = -350,
                .y      = 425,
                .width  = 75,
                .height = 75
            }
        },

        [MINI_GAME_ID_SOLITAIRE] = {
            .name = "Solitaire",
            .color = {0, 160, 80,  200},
            .hitbox = {
                .x      = -125,
                .y      = 425,
                .width  = 75,
                .height = 75
            }
        },

        [MINI_GAME_ID_SUIKA] = {
            .name = "Suika",
            .color = {220, 80, 0,  200},
            .hitbox = {
                .x      = 100,
                .y      = 425,
                .width  = 75,
                .height = 75
            }
        },

        [MINI_GAME_ID_BOWLING] = {
            .name = "Bowling",
            .color = {140, 0, 200, 200},
            .hitbox = {
                .x      = 325,
                .y      = 425,
                .width  = 75,
                .height = 75
            }
        },

        [MINI_GAME_ID_GOLF] = {
            .name = "Golf 3D",
            .color = { 20, 160,  50, 200},
            .hitbox = {
                .x      = 550,
                .y      = 425,
                .width  = 75,
                .height = 75
            }
        },

        [MINI_GAME_ID_SNAKE] = {
            .name = "Snake",
            .color = {  0, 200,  80, 200},
            .hitbox = {
                .x      = 775,
                .y      = 425,
                .width  = 75,
                .height = 75
            }
        },

        [MINI_GAME_ID_BINGO] = {
            .name = "Bingo",
            .color = {255, 200,   0, 200},
            .hitbox = {
                .x      = 1000,
                .y      = 425,
                .width  = 75,
                .height = 75
            }
        },

        [MINI_GAME_ID_BLOCKBLAST] = {
            .name = "BlockBlast",
            .color = { 60,  60, 200, 200},
            .hitbox = {
                .x      = 1225,
                .y      = 425,
                .width  = 75,
                .height = 75
            }
        },
    };

    memcpy(gameRef->subGameManager.gameZones, gameHitboxes, sizeof(gameHitboxes));

    /** Current active scene (lobby or one of the mini-games) */
    gameRef->subGameManager.currentScene = MINI_GAME_ID_LOBBY;
    
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

    if (gameTime > 3.0f) {
        updatePlayer(&game->player, platforms, platformCount, dt);
    }
    
    Vector2 desiredTarget = game->player.position;
    if (game->player.onGround && game->player.position.y > GROUND_Y - 70.0f) {
        desiredTarget.y -= 135.0f;
    } else {
        desiredTarget.y -= game->player.radius * 1.5f;
    }

    game->cam.target = Vector2Lerp(
        game->cam.target,
        desiredTarget,
        0.1f
    );

    toggleSkinMenu(game);

    if (game->playerVisuals.isTextureMenuOpen) {
        choosePlayerTexture(&game->player, game);
    }

    lobby_updateGrass(&game->player, GetFrameTime(), gameTime, game->cam);
    lobby_updateAtmosphericEffects(dt, &game->player, game->cam);

    // Update params menu (settings button clicks)
    paramsMenu_update(&paramsMenu);

    // Collision check with game zone
    for (u8 i = 1; i < __miniGameIdCount; ++i) {
        if (CheckCollisionCircleRec(game->player.position, game->player.radius, game->subGameManager.gameZones[i].hitbox)) {
            if (IsKeyPressed(KEY_E)) {
                game->subGameManager.currentScene = i;
                game->subGameManager.needGameInit = true;
            }
        }
    }

    BeginDrawing(); {
        ClearBackground(RAYWHITE);

        BeginMode2D(game->cam); {
            lobby_drawStarryBackground(game->player.position, game->cam);

            lobby_drawTree();
            
            lobby_drawPlatforms(platforms, platformCount);

            lobby_drawPlayer(game);
            
            lobby_drawWorldBoundaries(&game->player);
            
            lobby_drawGrass(&game->player, game->cam);

            lobby_drawGameZones(game);

            lobby_drawAtmosphericEffects();
        } EndMode2D();

        lobby_drawScreenEffects(&game->player);

        lobbyTextXPos = (systemSettings.video.width - MeasureText("Multi-Mini-Games", 20)) / 2.0f;
        DrawText("Multi-Mini-Games", lobbyTextXPos, 20, 20, PURPLE);

        lobby_drawSkinButton();

        // Draw params menu (settings button)
        paramsMenu_draw(&paramsMenu);

        if (game->playerVisuals.isTextureMenuOpen) {
            lobby_drawMenuTextures(game);
        }
    } EndDrawing();

    return OK;
}

Error_Et lobby_freeGame(LobbyGame_St** game) {
    if (game == NULL || *game == NULL) return ERROR_NULL_POINTER;
    LobbyGame_St* gameRef = *game;

    for (u8 i = 1; i < __miniGameIdCount; ++i) {
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